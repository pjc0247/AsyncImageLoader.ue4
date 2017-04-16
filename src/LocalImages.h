#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "LocalImages.generated.h"

USTRUCT(BlueprintType)
struct FLocalImage {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString path;
    UPROPERTY(BlueprintReadOnly)
    FString filename;
    UPROPERTY(BlueprintReadOnly)
    UTexture2D *texture;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FLoadThumbnailDelegate, FLocalImage, image);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLoadTexture2DDelegate, UTexture2D*, texture);

UCLASS()
class ASYNCLOADER_API ULocalImages : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "LocalResource")
    static void LoadThumbnailsFromMyPictures(int offset, int count, FLoadThumbnailDelegate callback);

    UFUNCTION(BlueprintCallable, Category = "LocalResource", meta=(DisplayName="LoadTexture2D Async"))
    static void LoadTexture2DAsync(const FString path, FLoadTexture2DDelegate callback);

private:
    static bool LoadImageData(const FString &path,
        const TArray<uint8> *raw, int &width, int &height);

    static TArray<FColor> ConvertRawToFColorRaw(const TArray<uint8> &src);
    static TArray<FColor> ResizeImage(const TArray<FColor> &src,
        int srcWidth, int srcHeight, int dstWidth, int dstHeight);
    
    static UTexture2D *CreateTextureFromRaw(const TArray<FColor> &src, int width, int height);
};
