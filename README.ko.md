# PillTaskbar

PillTaskbar는 Windows 11 작업표시줄을 화면 가운데에 떠 있는 알약 모양으로 바꿉니다. 작업표시줄 폴더, 사용량 표시, 아이콘 크기, 자동 숨김, 시스템 트레이 옵션도 함께 들어 있습니다.

소스 파일 하나로 두 가지 방식으로 쓸 수 있습니다.

- Windhawk 모드로 쓰기. `pill-taskbar.wh.cpp`를 Windhawk에 넣고 그곳에서 컴파일합니다.
- Bloom 애드온으로 쓰기. Windhawk 없이 같은 모드가 돌아갑니다. 작은 호스트 프로그램이 모드를 작업표시줄 프로세스에 넣고 계속 지켜봅니다. 이 저장소는 그 애드온의 소스입니다.

## 애드온이 기호를 얻는 방법

모드는 작업표시줄 DLL의 내부 함수를 훅하기 때문에 그 DLL의 기호 파일이 필요합니다. 처음 실행할 때 호스트가 해당 DLL에 맞는 Microsoft 기호 파일을 받고, 여기서 얻은 오프셋을 내 PC의 캐시에 저장합니다. Microsoft의 파일은 이 저장소와 릴리스 zip에 들어 있지 않습니다. 인터넷에 연결되지 않으면 첫 실행을 끝낼 수 없습니다.

## 소스에서 빌드하기

`x86_64-w64-mingw32`를 대상으로 하는 clang이 필요합니다. llvm-mingw도 되고 Windhawk에 들어 있는 컴파일러도 됩니다. Python 3과 Git Bash 같은 bash 셸도 필요합니다.

```
export CXX=/path/to/llvm-mingw/bin/clang++.exe   # 생략하면 Windhawk 컴파일러를 씁니다
bash tools/build-addon.sh
```

결과물은 `build/addon`에 생깁니다. C++ 런타임을 정적으로 묶기 때문에 바이너리 옆에 따로 DLL이 필요하지 않습니다.

커밋하기 전에 `python tools/check-public.py`를 실행하세요. 개인 경로, 메일 주소, 디버그 기호처럼 공개하면 안 되는 것을 찾아 줍니다.

## 라이선스

PillTaskbar는 GNU General Public License 버전 3 전용(GPL-3.0-only)으로 공개하는 자유 소프트웨어입니다. `LICENSE`를 보세요.

모드의 일부는 다른 Windhawk 모드에서 가져왔고, 애드온에는 MinHook(BSD-2-Clause)과 Zydis(MIT)가 들어 있습니다. 저작자 표시와 라이선스는 `THIRD_PARTY.md`와 `THIRD_PARTY_NOTICES.txt`에 있습니다.

이 프로그램은 어떤 보증도 하지 않습니다. explorer.exe 안의 코드를 바꾸고, 빠른 설정을 트레이 쪽에 놓으려고 ShellHost.exe 안의 코드도, 알림 센터 슬라이드를 끄려고 ShellExperienceHost.exe 메모리의 애니메이션 문자열 두 개도 바꾸므로 사용에 따른 책임은 사용자에게 있습니다. Windows와 Windhawk는 각 소유자의 상표이며, 이 프로젝트는 Microsoft나 Windhawk 제작자와 관계가 없습니다.
