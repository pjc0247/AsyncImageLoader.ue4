AsyncImageLoader.ue4
====
Asynchronous texture loader for Unreal4.<br>
Supports both C++ and Blueprint.

Usage
----
__Load thumbnails from 'MyPictures' directory__<br>
<img src="asyncLoader.PNG" width="450px" /><br>
내 그림 폴더의 사진 목록을 가져옵니다. 로드된 텍스쳐는 온전한 이미지 그대로가 아닌 __128x128__ 텍스쳐로 로드되게 됩니다. (썸네일 용)<br>
* __Offset__ : Start offset (to implement paging. If you don't need paging, just set it to 0)
* __Count__ : Maximun count to load
* __Callback__ : Will be called every single thumbnail which is successfully loaded. (it can be called 0 ~ `Count`)

<br>

__Load single image asynchronous__<br>
<img src="asyncLoader2.PNG" width="180px" /><br>

