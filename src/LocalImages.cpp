#include "AsyncLoader.h"

#include <thread>

#include "Async.h"
#include "ImageUtils.h"
#include "ImageWrapper.h"
#include "RenderUtils.h"

#include "LocalImages.h"

#ifdef PLATFORM_WINDOWS
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#endif

using namespace std;

void ULocalImages::LoadThumbnailsFromMyPictures(int offset, int count, FLoadThumbnailDelegate callback) {
    thread([offset, count, callback]() {
        char picPath[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, picPath);

        IFileManager &fm = IFileManager::Get();
        TArray<FString> files;
        fm.FindFiles(files, ANSI_TO_TCHAR(picPath), TEXT("*.*"));
        
        for (int i=offset;i<offset + count;i++) {
            if (i >= files.Num()) break;

            FString filename(files[i]);
            FString absPath(ANSI_TO_TCHAR(picPath));
            absPath += "\\";
            absPath += filename;

            UE_LOG(LogTemp, Warning, TEXT("Load : %s"), *absPath);

            TArray<uint8> raw;
            int width, height;
            if (LoadImageData(absPath, raw, width, height) == false)
                continue;

            auto resized = ResizeImage(
                ConvertRawToFColorRaw(raw),
                width, height, 128, 128);
            
            AsyncTask(ENamedThreads::GameThread, [callback, filename, absPath, resized]() {
                auto texture = CreateTextureFromRaw(resized, 128, 128);

                FLocalImage localImage;
                localImage.path = absPath;
                localImage.filename = filename;
                localImage.texture = texture;

                callback.ExecuteIfBound(localImage);
            });
        }
    }).detach();
}

void ULocalImages::LoadTexture2DAsync(const FString path, FLoadTexture2DDelegate callback) {
    if (FPaths::FileExists(path) == false) {
        callback.ExecuteIfBound(nullptr);
        return;
    }

    thread([path, callback]() {
        TArray<uint8> raw;
        int width, height;

        if (LoadImageData(path, raw, width, height)) {
            auto rawConverted = ConvertRawToFColorRaw(raw);

            AsyncTask(ENamedThreads::GameThread, [rawConverted, width, height, callback]() {
                auto texture = CreateTextureFromRaw(rawConverted, width, height);

                callback.ExecuteIfBound(texture);
            });
        }
        else {
            AsyncTask(ENamedThreads::GameThread, [callback]() {
                callback.ExecuteIfBound(nullptr);
            });
        }
    }).detach();
}

bool ULocalImages::LoadImageData(const FString &path,
    TArray<uint8> &raw, int &width, int &height) {

    auto &imageWrapper = FModuleManager::LoadModuleChecked<IImageWrapperModule>(TEXT("ImageWrapper"));

    TArray<uint8> fd;
    if (!FFileHelper::LoadFileToArray(fd, *path)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *path);
        return false;
    }

    auto format = imageWrapper.DetectImageFormat(fd.GetData(), fd.Num());
    if (format == EImageFormat::Invalid) {
        UE_LOG(LogTemp, Error, TEXT("Unrecognized image file format: %s"), *path);
        return false;
    }

    auto wrapper = imageWrapper.CreateImageWrapper(format);
    const TArray<uint8> *rawPtr = nullptr;

    wrapper->SetCompressed(fd.GetData(), fd.Num());
    wrapper->GetRaw(ERGBFormat::BGRA, 8, rawPtr);
    if (rawPtr == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("Failed to decompress image file: %s"), *path);
        return false;
    }
    
    raw.Insert(*rawPtr, 0);
    width = wrapper->GetWidth();
    height = wrapper->GetHeight();

    return true;
}

TArray<FColor> ULocalImages::ConvertRawToFColorRaw(const TArray<uint8> &src) {
    TArray<FColor> dst;
    dst.Reset(src.Num() / 4);

    for (int i = 0; i < src.Num() / 4; i++) {
        dst.Add(FColor(
            (src)[i * 4], (src)[i * 4 + 1], (src)[i * 4 + 2],
            (src)[i * 4 + 3]));
    }

    return dst;
}
TArray<FColor> ULocalImages::ResizeImage(const TArray<FColor> &src,
    int srcWidth, int srcHeight, int dstWidth, int dstHeight) {

    TArray<FColor> dst;
    dst.Reset(dstWidth * dstHeight);

    FImageUtils::ImageResize(
        srcWidth, srcHeight, src,
        dstWidth, dstHeight, dst, true);

    return dst;
}
UTexture2D *ULocalImages::CreateTextureFromRaw(const TArray<FColor> &src, int width, int height) {
    auto texture = UTexture2D::CreateTransient(128, 128, PF_B8G8R8A8);
    auto mipData = static_cast<uint8*>(texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

    uint8* dstPtr = nullptr;
    const FColor* srcPtr = NULL;
    for (int32 y = 0; y<height; y++) {
        dstPtr = &mipData[(height - 1 - y) * width * sizeof(FColor)];
        srcPtr = const_cast<FColor*>(&src[(height - 1 - y) * width]);
        for (int32 x = 0; x<width; x++) {
            *dstPtr++ = srcPtr->B;
            *dstPtr++ = srcPtr->G;
            *dstPtr++ = srcPtr->R;
            if (true) {
                *dstPtr++ = srcPtr->A;
            }
            else {
                *dstPtr++ = 0xFF;
            }
            srcPtr++;
        }
    }

    texture->PlatformData->Mips[0].BulkData.Unlock();
#undef UpdateResource
    texture->UpdateResource();

    return texture;
}
