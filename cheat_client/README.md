# Client ReadMe

![dapdap](https://user-images.githubusercontent.com/37067785/58430780-c5729e00-80e5-11e9-94a4-599fc37268fc.gif)



### 환경 구성

운영체제: Microsoft Windows 10 Pro  
개발도구: Visual Studio 2017(SDK: 10.0.17763.0)



### 기초 설정

__1. git clone nanogui__

  현재 디렉토리에 git clone  
  ```
git clone https://github.com/wjakob/nanogui.git --recursive
  ```

__2. 프로젝트 빌드__

  CMake 를 활용하여 프로젝트 빌드  
  CMake 에 관한 자세한 사용 방법은 [부록 - CMake](#-cmake-) 참조

__3. 솔루션 빌드__

  Visual Studio 2017을 사용하여 nanogui 프로젝트 __솔루션 빌드(B)__ 진행(Debug/Release x86 진행)

<p align="center">
  <image src="https://user-images.githubusercontent.com/37067785/58463450-d6b3bd00-816e-11e9-8c0e-d46933d97711.png" width="50%" height="50%">
  <h6 align="center">[그림1] VS2017로 솔루션 빌드</h6>
</p>



## 부록

### [ CMake ]

__1. 다운로드__

  링크: https://cmake.org/download/

<p align="center">
  <image src="https://user-images.githubusercontent.com/37067785/58431443-bccf9700-80e8-11e9-804e-aa1ee093a16f.png" width="75%" height="75%">
  <h6 align="center">[그림2] CMake 다운로드</h6>
</p>

  다운로드한 CMake.msi 파일 설치

<p align="center">
  <image src="https://user-images.githubusercontent.com/37067785/58457467-bf220780-8161-11e9-8a0e-b726be5192ed.png" width="50%" height="50%">
  <h6 align="center">[그림3] CMake 설치</h6>
</p>

__2. GUI 실행__

  2-1. 현재 폴더에 __build__ 폴더 생성(.sln, .vcxproj, .lib, .exe 등 빌드시 생성되는 파일이 담기는 경로)  
  2-2. cmake-gui 실행  
  2-3. Where is the source Code: git clone 한 __nanogui__ 경로 입력  
  2-4. Where to build the binaries: 생성한 __build__ 경로 입력  
  2-5. __Configure__ 버튼 클릭  
  2-6. Specify the generator for this project: __Visual Studio 15 2017__ 로 설정  
  2-7. __Finish__ 버튼 클릭
<p align="center">
  <image src="https://user-images.githubusercontent.com/37067785/58466897-90159100-8175-11e9-953e-d0a53c19010b.png" width="70%" height="70%">
  <h6 align="center">[그림4] CMake 실행</h6>
</p>

  2-8. Configuring done 메시지가 확인되면, __Generate__ 버튼 클릭



### [ 발생할 수 있는 오류 ]

#### - C3861

  에러가 발생하는 파일에 대해 다음 구문 추가  
  ```c++
#include <winnt.rh>
  ```



