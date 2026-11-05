# 출처와 라이선스

PillTaskbar(`pill-taskbar.wh.cpp`)는 GNU General Public License v3.0으로 공개한다. 아래 GPL-3.0 모드의 코드를 옮겨 고쳐 썼기 때문이다.

| 출처 | 판 | 작성자 | 라이선스 | 옮긴 것 |
|---|---|---|---|---|
| [windows-11-taskbar-styler](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-taskbar-styler.wh.cpp) | 1.11 | m417z | GPL-3.0 | WindowGlass 테마 수치, 클릭 통과(`SetWindowRgn`) 방식, 블러 브러시 구성(GaussianBlur + Flood + Composite, 테마 색 프록시). 블러 브러시는 원래 TranslucentTB의 XamlBlurBrush(GPL-3.0)에 바탕을 둔다 |
| [taskbar-tray-system-icon-tweaks](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-tray-system-icon-tweaks.wh.cpp) | 1.3.1 | m417z | GPL-3.0 | `IconView` 생성자 훅과 `Loaded`, 트리 탐색 도우미, `TaskbarHost`로 XamlRoot 얻기, 트레이 아이콘 글리프 표, 아이콘 숨김과 간격 보정, 알림 벨 처리 |
| [taskbar-icon-size](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-icon-size.wh.cpp) | 1.3.11 | m417z | GPL-3.0 | 높이, 아이콘 크기, 버튼 폭 훅과 멤버 오프셋 찾기(동적 아이콘 크기 빌드, x64 경로) |
| [taskbar-auto-hide-custom-activation-area](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-custom-activation-area.wh.cpp) | 1.0 | m417z | GPL-3.0 | `ViewCoordinator::UpdateIsExpanded`의 이유 값(포인터 진입) 걸러 내기와 `GetMessagePos`로 나오는 구간 판정 |
| [taskbar-auto-hide-speed](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-speed.wh.cpp) | 1.1 | m417z | GPL-3.0 | `SharedAnimations`의 펼치기/접기 `KeyFrameAnimation.Duration` 바꾸기 |
| [taskbar-on-top](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-on-top.wh.cpp) / [taskbar-vertical](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-vertical.wh.cpp) | 1.3 / 1.5 | m417z | GPL-3.0 | wil 기능 `Feature_61970341` 끄기 훅(위쪽 작업표시줄의 자동 숨김 허용) |
| [taskbar-ai-quota](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-ai-quota.wh.cpp) | 1.6.5 | Cleroth | MIT | 보조 작업표시줄(`CSecondaryTaskBand`)의 XamlRoot 얻는 경로 |
| taskbar-folders(공개하지 않은 모드) | 3.15 | PillTaskbar 작성자 | 자작, 이 저장소에서 GPL-3.0으로 공개 | 모드 전체를 `folders` 구역으로 옮김(앱 끌어 합치기, 폴더 창과 메뉴, 폴더 안 앱의 창 묶기, 탐색기에서 넣기, 미리보기 아이콘). 작업표시줄 스레드 `DispatcherQueue`를 훅에서 기억해 두는 방식도 여기서 왔다. 아래 다섯 줄은 taskbar-folders가 다른 곳에서 가져온 것 |
| [taskbar-grouping](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-grouping.wh.cpp) | 1.3.12 | m417z | GPL-3.0 | `RESOLVEDWINDOW` 구조체 배치, `CTaskBand::_HandleItemResolved`에서 앱 ID를 바꿔 창을 다른 버튼으로 보내는 방식, 스냅 그룹 항목을 건너뛰는 이유(탐색기 충돌) |
| [taskbar-icon-separators](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-icon-separators.wh.cpp) | 1.0.33 | meteoni | GPL-3.0 | `IPinnedList3`, `IPinManagerInterop2/3` 인터페이스 배치와 CLSID/IID, `PINNEDLISTMODIFYCALLER` 값 |
| [mac-island-dock](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/mac-island-dock.wh.cpp) | 2.1.0 | caliberda | GPL-3.0 | `IApplicationResolver8`의 CLSID/IID와 `GetAppIDForWindow` 자리. taskbar-folders 주석은 file-explorer-remove-suffixes를 들었지만 그 모드에는 라이선스 표기가 없어, 같은 배치가 있는 이 모드를 출처로 적는다 |
| [start-everything](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/start-everything.wh.cpp) / [macos-style-dock](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/macos-style-dock.wh.cpp) | 2.1.1 / 1.0 | bardelyne / Zach Vaughn | GPL-3.0 | 팝업 창(`Xaml_WindowedPopupClass`)에 `DWMWA_USE_HOSTBACKDROPBRUSH`(17)를 켜는 방식 / 오른쪽 클릭을 받는 두 기호(`TaskbarResources::OnTaskListButtonContextRequested`, `TaskListButtonHandlers::HandleContextRequested`)와 탐색기 파일 놓기를 `DragEventArgs`로 받는 방식 |
| [figma-squircle](https://github.com/phamfoo/figma-squircle) / [HeroUI](https://github.com/heroui-inc/heroui) | — | Tien Pham / Next UI Inc. | MIT | 폴더 창 모서리의 corner smoothing 계산(`getPathParamsForCorner`) / 폴더 창 색(HeroUI v3 기본 테마 토큰) |

라이선스 표기가 없는 모드의 코드는 옮기지 않았다.

시작 버튼 기능(`start`)은 아래 두 모드의 방식만 보고 새로 썼다. 코드는 옮기지 않았다.

| 출처 | 판 | 작성자 | 라이선스 | 참고한 방식 |
|---|---|---|---|---|
| [start-button-replacer](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/start-button-replacer.wh.cpp) | 0.12.4 | Ender | GPL-3.0-only | 순정 아이콘(`AnimatedVisualPlayer#Icon`)을 투명하게 하고 그 부모 Panel에 그림을 얹기, `ExperienceToggleButton::UpdateButtonPadding`에서 다시 붙이기 |
| [hide-start-button](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/hide-start-button.wh.cpp) | 1.0 | ptrkhh | 표기 없음 | 시작 버튼을 폭 0으로 배치해 자리를 없애기 |
| [taskbar-elastic-pill](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-elastic-pill.wh.cpp) | 2.0.1 | Lockframe | 표기 없음 | 실행 중 표시(`Rectangle#RunningIndicator`)를 `TaskListButton::UpdateVisualStates` 뒤마다 Opacity로 숨기기 |

## 로고

이 저장소와 배포물에는 회사 로고를 싣지 않는다. 사용량 표시의 아이콘은 사용자가 Bloom 설정에서 자기 PC의 파일(.svg, .png, .ico)을 고를 때만 보이고, 고르지 않으면 계정 종류의 첫 글자(C, G)를 그린다.

| 그림 | 작성자 | 가져온 곳 | 라이선스 |
|---|---|---|---|
| Bloom 꽃 | Bloom 작성자 | Bloom의 `tools\make_icon.ps1`에 있는 꽃잎 식(타일 없이 72 정사각형으로 다시 계산, `start::kPetals`) | MIT(Bloom) |

## figma-squircle (MIT)

```
Copyright (c) 2021 Tien Pham
```

## HeroUI (MIT)

```
Copyright (c) 2020 Next UI Inc.
```

두 원본 모두 아래 taskbar-ai-quota 절과 같은 MIT 문구를 쓴다.

## taskbar-ai-quota (MIT)

보조 작업표시줄의 XamlRoot를 얻는 경로를 참고했다. 원본 머리에는 `@license MIT`와 작성자 Cleroth만 적혀 있어, 그 표시를 아래 MIT 문구와 함께 싣는다.

```
Copyright (c) Cleroth

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```


## 애드온판(Windhawk 없이 도는 판)에 들어간 것

Bloom 애드온으로 빌드한 `PillTaskbarRuntime.dll`, `PillTaskbar.dll`, `PillTaskbarHost.exe`에는 아래가 함께 들어간다. 전문은 배포 폴더의 `THIRD_PARTY_NOTICES.txt`(빌드가 `third_party`의 라이선스 파일을 모아 만든다)에 있다.

| 구성 | 판 | 라이선스 | 쓰임 |
|---|---|---|---|
| [MinHook](https://github.com/TsudaKageyu/minhook) (HDE64 포함) | 1.3.4 | BSD-2-Clause | 함수 훅(`Wh_SetFunctionHook`, `Wh_ApplyHookOperations`) |
| [Zydis](https://github.com/zyantific/zydis) / Zycore | 4.1.0 합본 | MIT | `Wh_Disasm`(Intel 문법 글자) |
| mingw-w64 런타임 | 빌드에 쓴 llvm-mingw에 든 것 | `COPYING.MinGW-w64-runtime.txt` | C 런타임 시작 코드 |
| libc++, libunwind, compiler-rt | LLVM 20 | Apache-2.0 WITH LLVM-exception | 정적으로 묶은 C++ 런타임 |

런타임의 헤더(`runtime/include`)는 Windhawk 모드 API와 같은 함수 이름과 시그니처를 쓰지만 새로 썼다. Windhawk(GPL-3.0) 엔진의 동작(콜백 순서, 훅 적용 시점, 기호 캐시 서식)을 따랐다.
