AsyncImageLoader.ue4
====
비동기로 이미지 로드하기

Usage
----
__내 그림 폴더에서 사진 썸네일 가져오기__<br>
<img src="asyncLoader.PNG" width="450px" /><br>
내 그림 폴더의 사진 목록을 가져옵니다. 로드된 텍스쳐는 온전한 이미지 그대로가 아닌 __128x128__ 텍스쳐로 로드되게 됩니다. (썸네일 용)<br>
* __Offset__ : n번째 파일부터 로드할지 지정합니다.
* __Count__ : 최대 몇개를 로드할지 지정합니다.
* __Callback__ : 썸네일이 1장 로드될 때마다 호출됩니다. 폴더의 파일 갯수에 따라 0번 혹은 여러번 호출될 수 있습니다.

<br>

__단일 텍스쳐 비동기 로드하기__<br>
<img src="asyncLoader2.PNG" width="180px" /><br>

