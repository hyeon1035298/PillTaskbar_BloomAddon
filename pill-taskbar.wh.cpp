// ==WindhawkMod==
// @id              pill-taskbar
// @name            PillTaskbar
// @description     Pill-shaped taskbar with a blurred background and click-through sides
// @version         0.1
// @author          Hyeon
// @include         explorer.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ld2d1 -ld3d11 -ldwmapi -ldwrite -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshcore -lshlwapi -luuid -lversion -lwindowscodecs
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Source code is published under The GNU General Public License v3.0.
// Third-party sources and licenses: THIRD_PARTY.md next to this file.

// ==WindhawkModReadme==
/*
# PillTaskbar

작업표시줄을 가운데 알약 모양으로 바꿉니다.

- **알약**: 작업 영역과 트레이를 가운데로 모아 유리 배경(블러, 옅은 틴트, 빛을 받는
  가장자리, 위 반사광과 아래 집광) 하나로 이어 붙입니다. 알약 양옆의 빈 곳은 클릭이 바탕 화면으로 통과합니다.
- **위치**: 작업표시줄을 화면 위나 아래에 두고, 알약을 가장자리의 왼쪽, 가운데, 오른쪽에 붙입니다.
  Windows 설정은 바꾸지 않아서 끄면 Windows 설정 위치로 돌아갑니다.
- **트레이**: 볼륨, 네트워크 같은 시스템 아이콘과 알림 벨을 숨기고, 시계를 시간만 보이게 합니다.
  빠른 설정(Win+A)과 알림 센터·달력(Win+N, 시계)은 화면 오른쪽 끝이 아니라 알약 트레이 쪽에 열립니다.
  작업표시줄이 위에 있으면 알림 센터도 아래로 내려가지 않고 위쪽에 붙습니다.
  빠른 설정은 ShellHost.exe가 띄우므로 모드가 그 프로세스에도 올라갑니다(거기서는 패널 자리만 정합니다).
  알림 센터는 오른쪽에서 밀려 들어오지 않고 제자리에 나타납니다. ShellExperienceHost.exe에는 모드를 넣지 않고,
  explorer가 그 프로세스의 슬라이드 식 문자열 두 개만 같은 길이로 바꿔 두었다가 끌 때 되돌립니다.
- **크기**: 작업표시줄 높이, 아이콘 크기, 아이콘 간격을 바꿉니다. explorer를 다시 시작하지 않아도
  바로 반영됩니다.
- **시작 버튼**: Windows 시작 버튼을 숨기거나, 그 자리에 Bloom 런처를 여는 꽃 버튼을 둡니다. 꽃 버튼을
  누르면 Bloom이 열리고, 열려 있으면 닫힙니다. 오른쪽 클릭 메뉴는 그대로이고, 시작 메뉴는 Win 키로 열립니다.
- **자동 숨김**: Windows 설정에서 자동 숨김을 켜면 위쪽 작업표시줄도 숨겨지고, 화면 위 가장자리
  어디에서 나올지, 나오고 들어가는 지연과 애니메이션 길이를 정할 수 있습니다. Bloom 런처가 열려 있는 동안에는
  숨은 작업표시줄도 나와 있습니다. 알림 센터(Win+N, 시계)가 열려 있으면 작업표시줄과 한 덩어리로 움직입니다.
  포인터가 둘 중 어디에 있든 작업표시줄이 나와 있고, 둘 다 벗어나 작업표시줄이 들어가면 알림 센터도 닫힙니다.
- **미디어**: 재생 중인 곡의 앨범 아트, 제목, 이전/재생/다음 단추를 트레이 앞에 보여 줍니다.
- **사용량**: Bloom이 내보낸 AI 사용량을 트레이 앞에 작은 막대로 보여 줍니다. Bloom 파일이 없으면
  아무것도 보이지 않습니다.
- **숨김 아일랜드**: 작업표시줄이 자동 숨김으로 숨어 있는 동안 모니터마다 그 가장자리(위 또는 아래) 가운데에 작은 검은 아일랜드를 둡니다.
  아일랜드를 클릭하면(설정에 따라 포인터를 대기만 해도) 아일랜드가 작업표시줄 모양으로 커지며 작업표시줄이 나오고,
  작업표시줄이 들어가면 다시 아일랜드로 줄어듭니다. 아일랜드 위에 포인터를 올리면 아일랜드가 설정한 만큼 투명해집니다.
  아일랜드에는 시계와 재생 중인 곡, AI 사용량 가운데 고른 것을 보여 줍니다. Ctrl을 누른 채 끌면 가장자리를 따라 옮겨지고
  모니터마다 그 자리를 기억하며, Ctrl을 누른 채 우클릭하면 가운데로 돌아갑니다. 자동 숨김 보조를 같이 켜야 동작합니다.
- **폴더**: 고정한 앱을 끌어 다른 앱 한가운데에 잠깐 두면 그 앱이 커지고, 그때 놓으면 둘이 폴더가
  됩니다. 폴더를 누르면 안의 앱이 한 줄로 뜨고, 폴더 안 앱의 창은 따로 버튼을 만들지 않고 폴더
  버튼에 묶입니다. 폴더를 오른쪽 클릭하면 이름과 아이콘을 바꾸거나 폴더를 풀 수 있습니다. 폴더는
  설정한 위치(기본 `%USERPROFILE%\TaskbarFolders`)의 하위 폴더입니다. 고정하지 않고 켠 앱 가운데
  어느 폴더에도 없는 앱은 시계 모양 '실행 중' 폴더에 저절로 모이고, 앱을 끄면 빠집니다.

Windows 11 XAML 작업표시줄(가로)만 지원합니다. 작업표시줄이 세로면 스스로 물러납니다.
windows-11-taskbar-styler와 같이 켜지 마세요(같은 속성을 둘이 씁니다).
폴더를 켜 둔 동안에는 Taskbar Folders 모드를 꺼 두세요(같은 폴더와 창 묶기를 둘이 맡습니다).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- pill:
  - enabled: true
    $name: 알약 모양
    $description: 끄면 Windows 기본 작업표시줄 모양으로 돌아갑니다
  - edge: windows
    $name: 작업표시줄 위치
    $description: 화면 위나 아래. 레지스트리의 Windows 설정은 바꾸지 않아서, 이 기능을 끄거나 모드를 끄면 Windows 설정 위치로 돌아갑니다. 위에 두고 자동 숨김을 쓰려면 자동 숨김 보조도 켜 두세요
    $options:
    - windows: Windows 설정 따름
    - top: 위
    - bottom: 아래
  - align: center
    $name: 알약 정렬
    $description: 펼친 알약을 가장자리의 어디에 둘지
    $options:
    - left: 왼쪽
    - center: 가운데
    - right: 오른쪽
  - style: bloom
    $name: 모양
    $options:
    - bloom: Bloom (유리에 옅은 윤곽선)
    - glass: 글래스모피즘 (빛을 받는 가장자리, 위 반사광과 아래 집광)
    - windowGlass: WindowGlass (테두리 없는 유리)
  - theme: system
    $name: 테마
    $description: 작업표시줄과 그 안의 아이콘, 사용량, 폴더의 라이트·다크. Bloom 애드온으로 쓰면 Bloom의 테마를 따릅니다
    $options:
    - system: Windows 작업표시줄 테마
    - dark: 다크
    - light: 라이트
  - glass: true
    $name: 유리 효과
    $description: 끄면 알약, 숨김 아일랜드가 바뀌는 모양, 폴더 창이 블러 없이 테마 색으로 칠해집니다. Bloom 애드온으로 쓰면 Bloom의 유리 효과를 따릅니다
  - cornerRadius: 25
    $name: 모서리 (px)
  - blur: 12
    $name: 배경 블러
  - tintOpacity: 50
    $name: 배경 틴트 불투명도 (%)
  $name: 알약
- tray:
  - hideVolume: true
    $name: 볼륨 아이콘 숨기기
  - hideNetwork: true
    $name: 네트워크 아이콘 숨기기
  - hideBattery: true
    $name: 배터리 아이콘 숨기기
  - hideMicrophone: true
    $name: 마이크 아이콘 숨기기
  - hideLocation: true
    $name: 위치 아이콘 숨기기
  - hideStudioEffects: true
    $name: Studio Effects 아이콘 숨기기
  - hideRecall: true
    $name: Recall 아이콘 숨기기
  - hideLanguageBar: true
    $name: 언어 표시줄 숨기기
  - hideLanguageExtras: true
    $name: 언어 보조 아이콘 숨기기
  - hideCopilot: true
    $name: Copilot 아이콘 숨기기
  - bell: hidden
    $name: 알림 벨
    $options:
    - shown: 항상 표시
    - whenNew: 새 알림이 있을 때만
    - hidden: 숨김
  - clock: time
    $name: 시계
    $options:
    - timeDate: 시간과 날짜
    - time: 시간만
    - hidden: 숨김
  - clockFormat: h24
    $name: 시간 표기
    $options:
    - h24: 24시간 (18:24)
    - h12: 12시간 (오후 6:24)
  - panelsNearTray: true
    $name: Windows 패널을 트레이 쪽에 열기
    $description: 빠른 설정(Win+A)과 알림 센터·달력(Win+N, 시계)을 화면 오른쪽 끝이 아니라 알약 트레이 쪽에 엽니다. 작업표시줄이 위에 있으면 알림 센터도 위쪽에 붙습니다. 끄면 Windows가 정한 자리에 열립니다
  - centerStill: true
    $name: 알림 센터 슬라이드 끄기
    $description: 알림 센터·달력(Win+N, 시계)이 화면 오른쪽에서 밀려 들어오지 않고 제자리에 바로 나타납니다. 끄면 Windows 애니메이션 그대로입니다
  $name: 트레이
  $description: 숨긴 아이콘은 자리도 남기지 않습니다. 레지스트리는 건드리지 않습니다
- size:
  - enabled: true
    $name: 크기 바꾸기
  - taskbarHeight: 56
    $name: 작업표시줄 높이 (px)
    $description: Windows 기본 48
  - iconSize: 32
    $name: 아이콘 크기 (px)
    $description: Windows 기본 24
  - iconSpacing: 14
    $name: 아이콘 간격 (px)
    $description: 앱 버튼 폭은 아이콘 크기 + 간격입니다 (Windows 기본 20)
  $name: 크기
  $description: Windows의 "작은 작업표시줄 단추" 설정과는 같이 쓰지 마세요
- start:
  - mode: bloom
    $name: 시작 버튼
    $options:
    - windows: 그대로
    - hidden: 숨기기 (시작 메뉴는 Win 키로)
    - bloom: Bloom 버튼 (누르면 Bloom을 열고 닫습니다)
  $name: 시작 버튼
  $description: 오른쪽 클릭 메뉴는 그대로입니다
- autohide:
  - enabled: true
    $name: 자동 숨김 보조
    $description: 자동 숨김을 켜고 끄는 것은 Windows 설정(작업 표시줄 동작)에서 합니다. 이 기능은 위쪽 작업표시줄도 숨게 하고 아래 값을 적용합니다
  - revealZone: 100
    $name: 나오는 구간 (%)
    $description: 가장자리 가운데(알약을 왼쪽이나 오른쪽에 붙였으면 그쪽 끝)에서 이만큼에서만 마우스로 나옵니다. 100이면 가장자리 전체
  - showDelay: -1
    $name: 나오기 지연 (ms)
    $description: -1이면 Windows 기본(마우스 호버 시간, 보통 400)
  - hideDelay: -1
    $name: 숨기 지연 (ms)
    $description: 마우스가 떠난 뒤 숨기까지. -1이면 Windows 기본(마우스 호버 시간)
  - animation: 0
    $name: 애니메이션 길이 (ms)
    $description: 0이면 Windows 기본
  - keepHiddenForBloom: false
    $name: Bloom 런처가 열려도 숨긴 채로
    $description: 끄면 Bloom 키나 Win 키로 Bloom 런처를 열 때 숨은 작업표시줄도 그 모니터에서 함께 나오고, 런처가 앞에 있는 동안 들어가지 않습니다
  - keepHiddenOnFlash: true
    $name: 앱이 깜박여도 숨긴 채로
    $description: 앱이 작업표시줄 단추를 깜박여 알릴 때 숨은 작업표시줄이 나오지 않습니다. 끄면 Windows처럼 나와서 그 앱을 열 때까지 들어가지 않습니다
  $name: 자동 숨김
- media:
  - enabled: true
    $name: 미디어 플레이어
    $description: 재생 중인 곡의 앨범 아트, 제목, 이전/재생/다음 버튼을 트레이 앞에 둡니다. 재생 중인 곡이 없으면 접힙니다
  - maxWidth: 100
    $name: 제목 최대 폭 (px)
    $description: 넘치는 제목은 …로 줄입니다
  $name: 미디어
- usage:
  - enabled: true
    $name: Bloom 사용량 표시
    $description: Bloom이 내보낸 AI 사용량 파일을 읽어 트레이에 계정마다 주간 사용 %와 막대로 보여 줍니다. 숫자는 Bloom의 쓴 만큼·남은 만큼 설정을 따르고, 우클릭하면 반대로 바뀝니다. 클릭하면 한도마다 자세한 창이 열립니다. Bloom 설정의 홈 화면 > AI 사용량에서 파일 내보내기를 켜야 하고, 내보내기 폴더가 없으면 아무것도 만들지 않습니다
  - logoColor: brand
    $name: 로고 색
    $description: 트레이와 아일랜드에 보이는 계정 아이콘(Bloom 설정 › AI 계정에서 고른 파일, 고르지 않으면 Bloom에 들어 있는 Anthropic·OpenAI 로고)
    $options:
    - mono: 단색 (글자의 흐린 색)
    - brand: 원래 색 (아이콘 파일의 색 그대로, 검정이나 흰색 한 가지로 된 로고는 글자색)
  - gapBefore: 14
    $name: 앱과의 간격 (px)
    $description: 마지막 앱 버튼과 사용량 사이에 더하는 간격입니다
  - gapAfter: 14
    $name: 오른쪽 간격 (px)
    $description: 사용량 끝에서 오른쪽 트레이 아이콘까지의 간격입니다
  - sessions: true
    $name: 자세한 창에 세션 보기
    $description: 사용량을 클릭해 여는 창에 이 PC의 Claude Code·Codex 세션(진행 상태, 이번 턴 토큰, 컨텍스트)을 보여 줍니다. 창이 열려 있는 동안에만 각 도구의 세션 기록 끝부분을 읽고, 대화 내용은 읽지 않습니다
  $name: 사용량
- island:
  - enabled: true
    $name: 숨김 아일랜드
    $description: 작업표시줄이 자동 숨김으로 숨어 있을 때 그 가장자리(위 또는 아래)에 작은 검은 아일랜드를 둡니다. 아일랜드를 클릭하거나(아래 설정) 포인터를 대면 아일랜드가 커지며 작업표시줄이 나옵니다. Ctrl을 누른 채 끌면 가장자리를 따라 옮기고, Ctrl을 누른 채 우클릭하면 가운데로 돌아갑니다. 자동 숨김 보조도 켜져 있어야 동작합니다
  - color: black
    $name: 아일랜드 색
    $options:
    - black: 검정
    - white: 흰색
  - showClock: true
    $name: 시계 보이기
  - showMedia: false
    $name: 재생 중인 곡 보이기
    $description: 미디어 플레이어 기능이 켜져 있을 때 앨범 아트와 제목을 보여 줍니다
  - showUsage: true
    $name: AI 사용량 보이기
    $description: Bloom 사용량 표시가 켜져 있고 Bloom 파일이 있을 때 보여 줍니다
  - showMemory: false
    $name: 메모리 사용률 보이기
    $description: 메모리 정리 애드온이 켜져 있을 때 그 작업표시줄 아이콘과 같은 모양으로 보여 줍니다
  - showCpu: false
    $name: CPU 사용률 보이기
    $description: 메모리 정리 애드온이 켜져 있을 때 보여 줍니다
  - clockAt: center
    $name: 시계 자리
    $description: 가운데 아일랜드, 또는 화면 왼쪽·오른쪽 모서리에 붙는 아일랜드
    $options:
    - center: 가운데
    - left: 왼쪽
    - right: 오른쪽
  - mediaAt: center
    $name: 재생 중인 곡 자리
    $options:
    - center: 가운데
    - left: 왼쪽
    - right: 오른쪽
  - usageAt: center
    $name: AI 사용량 자리
    $options:
    - center: 가운데
    - left: 왼쪽
    - right: 오른쪽
  - memoryAt: right
    $name: 메모리 사용률 자리
    $options:
    - center: 가운데
    - left: 왼쪽
    - right: 오른쪽
  - cpuAt: right
    $name: CPU 사용률 자리
    $options:
    - center: 가운데
    - left: 왼쪽
    - right: 오른쪽
  - height: 35
    $name: 높이 (px)
    $description: 100% 배율 기준, 20에서 60까지
  - width: 0
    $name: 폭 (px)
    $description: 100% 배율 기준 아일랜드 몸통의 폭, 1200까지. 0이면 보여 주는 내용에 맞춥니다. 내용이 넘치면 곡 제목을 …로 줄입니다
  - reveal: hover
    $name: 작업표시줄 부르기
    $description: 클릭이면 포인터를 대기만 해서는 나오지 않고 아일랜드를 눌러야 나옵니다. 가장자리에 대도 나오지 않고, 파일을 끌고 있을 때만 나옵니다
    $options:
    - hover: 포인터를 대면 바로
    - click: 아일랜드를 클릭해야
  - hoverTransparency: 100
    $name: 포인터를 올렸을 때 투명도 (%)
    $description: 아일랜드 위에 포인터가 있는 동안 아일랜드를 이만큼 투명하게 해 아래가 비쳐 보이게 합니다. 0이면 그대로, 100이면 보이지 않습니다. 클릭은 그대로 아일랜드가 받습니다
  $name: 숨김 아일랜드
- folders:
  - enabled: true
    $name: 작업표시줄 폴더
    $description: 고정한 앱을 다른 앱 위에 끌어 놓아 폴더로 묶습니다. 켜 둔 동안에는 Taskbar Folders 모드를 꺼 두세요
  - foldersPath: '%USERPROFILE%\TaskbarFolders'
    $name: 폴더 위치
    $description: 이 폴더의 하위 폴더 하나가 작업표시줄 폴더 하나가 됩니다
  - mergeDelay: 250
    $name: 합치기 지연 (ms)
    $description: 앱을 다른 앱 위에 이만큼 두어야 둘을 합칠 수 있습니다 (100~3000)
  - popupScale: 70
    $name: 폴더 크기 (%)
    $description: 열린 폴더와 그 메뉴를 글자와 아이콘까지 통째로 줄이거나 키웁니다 (50~150)
  - showLabels: false
    $name: 앱 이름 보이기
    $description: 열린 폴더에서 앱 아래에 이름을 둡니다. 끄면 칸이 아이콘 크기로 줄어듭니다
  - iconSize: 32
    $name: 폴더 안 아이콘 크기 (px)
    $description: 열린 폴더의 앱 아이콘 크기 (16~96)
  - appSpacing: 4
    $name: 앱 사이 간격 (px)
    $description: 열린 폴더의 앱 사이 (0~40)
  - popupSide: below
    $name: 폴더가 열리는 쪽
    $options:
    - auto: 작업표시줄 반대쪽
    - above: 작업표시줄 위
    - below: 작업표시줄 아래
  - popupGap: 8
    $name: 작업표시줄과의 거리 (px)
    $description: 작업표시줄과 열린 폴더나 그 메뉴 사이 (0~200)
  - cornerRadius: 20
    $name: 모서리 둥글기 (px)
    $description: 열린 폴더와 그 메뉴의 모서리 (0~40)
  - cornerSmoothing: 60
    $name: 모서리 부드러움 (%)
    $description: 0이면 보통 둥근 모서리, 60이면 애플 아이콘 같은 이어진 곡선입니다 (0~100)
  - autoRunning: true
    $name: 실행 중 폴더
    $description: 작업표시줄에 고정하지 않고 켠 앱 가운데 어느 폴더에도 없는 앱을 시계 모양 폴더 하나에 모읍니다. 앱을 끄면 빠지고, 그런 앱이 없으면 폴더도 없습니다
  - runningName: 실행 중
    $name: 실행 중 폴더 이름
    $description: 작업표시줄이 이 폴더 버튼에 붙이는 이름입니다
  $name: 폴더
*/
// ==/WindhawkModSettings==

// Settings Bloom keeps in a file of the add-on's data folder, which the mod
// writes too, appended to their group on Bloom's page (tools/make-schema.py).
// ==BloomFileSettings==
/*
[
  {
    "group": "island",
    "key": "island.positions",
    "type": "records",
    "default": {},
    "file": "prefs.json",
    "field": "islandPositions",
    "label": "name",
    "name": {"ko": "모니터별 아일랜드 위치", "en": "Island position per monitor"},
    "about": {
      "ko": "아일랜드를 Ctrl을 누른 채 끌어 옮기면 그 모니터의 자리가 여기 남습니다. 0이면 왼쪽 끝, 500이면 가운데, 1000이면 오른쪽 끝입니다",
      "en": "Where each monitor's island sits after you drag it with Ctrl held. 0 is the left end, 500 the middle, 1000 the right end"
    },
    "fields": [
      {
        "key": "x",
        "type": "int",
        "default": 500,
        "min": 0,
        "max": 1000,
        "name": {"ko": "가로 위치", "en": "Position along the edge"}
      }
    ]
  }
]
*/
// ==/BloomFileSettings==

// 구역 목차 (기능마다 한 구역에 모여 있다)
//   settings  Windhawk 설정 읽기와 theme 상수
//   core      UI 스레드, 작업표시줄 찾기, restore 목록, XAML 도우미
//   region    F1 클릭 통과 (SetWindowRgn)
//   pill      F1 알약 배경, 모서리, 두 조각 배치
//   widgets   F6·F7 위젯을 담는 트레이 앞 자리
//   tray      F3 트레이 시스템 아이콘, 알림 벨, 시계
//   size      F2 높이, 아이콘 크기, 버튼 폭
//   autohide  F4 위쪽 자동 숨김, 나오는 구간, 지연, 애니메이션
//   media     F6 미디어 플레이어 (GSMTC)
//   usage     F7 Bloom 주간 사용량
//   island    F8 숨김 아일랜드 (숨은 위·아래 작업표시줄을 불러내고 그 모양으로 바뀌는 검은 아일랜드)
//   folders   F9 작업표시줄 폴더 (앱 묶기, 폴더 창, 폴더 안 앱의 창 묶기), F10 실행 중 폴더
//   position  F11 작업표시줄 위·아래 (Windows 자체 위치 기능, 레지스트리는 읽기만)
//   hooks     켠 기능의 심볼만 훅으로 걸고 여러 기능이 쓰는 훅을 나눠 준다
//   entry     Wh_Mod* 진입점
// 의존 방향: hooks → 기능 구역 → core, settings. 기능 구역끼리는 region만 pill이 부르고,
// island는 autohide, media, usage의 상태를 쓰고, autohide는 island가 알려 주는 범위와
// start의 Bloom 런처 확인(LauncherAhead)을 쓴다.
// folders는 settings만 쓰고 작업 스레드와 작업표시줄 큐를 스스로 둔다.
// 알약 정렬(settings::PillAlign)은 pill, island, autohide가 같이 읽는다.

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <numbers>
#include <optional>
#include <regex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include <initguid.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <robuffer.h>
#include <shlobj.h>
#include <shlguid.h>  // shlobj.h leaves it out under INITGUID
#include <shlwapi.h>
#include <tlhelp32.h>
#include <wincodec.h>
#include <wrl/client.h>

#undef GetCurrentTime

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <chrono>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace wf = winrt::Windows::Foundation;

// ============================================================================
namespace settings {
// ============================================================================

enum class Style { kBloom, kGlass, kWindowGlass };
enum class Theme { kSystem, kDark, kLight };
enum class Bell { kShown, kWhenNew, kHidden };
enum class Clock { kTimeDate, kTime, kHidden };
enum class FolderSide { kAuto, kAbove, kBelow };
enum class LogoColor { kMono, kBrand };
enum class StartMode { kWindows, kHidden, kBloom };
enum class Edge { kWindows, kTop, kBottom };
enum class Align { kCenter, kLeft, kRight };  // an unknown value reads as the first
enum class Slot { kCenter, kLeft, kRight };   // the same

struct Settings {
    bool operator==(const Settings&) const = default;
    struct Pill {
        bool enabled;
        Edge edge;
        Align align;
        Style style;
        Theme theme;
        bool glass;  // else an opaque fill in the theme's color, no blur
        double cornerRadius;
        float blur;
        int tintOpacity;  // 0..100
        bool operator==(const Pill&) const = default;
    } pill;
    struct Tray {
        bool hideVolume, hideNetwork, hideBattery, hideMicrophone, hideLocation,
            hideStudioEffects, hideRecall, hideLanguageBar, hideLanguageExtras,
            hideCopilot;
        Bell bell;
        Clock clock;
        bool clock12;  // else 24 hours
        bool panelsNearTray;  // Quick Settings and the notification center
        bool centerStill;     // the notification center without its slide
        bool operator==(const Tray&) const = default;
    } tray;
    struct Size {
        bool enabled;
        int taskbarHeight;
        int iconSize;
        int iconSpacing;
        bool operator==(const Size&) const = default;
    } size;
    struct Start {
        StartMode mode;
        bool operator==(const Start&) const = default;
    } start;
        struct Autohide {
            bool enabled;
            int revealZone;  // 1..100, % of the edge around its middle
            int showDelay;   // ms, -1 = Windows
            int hideDelay;   // ms, -1 = Windows
            int animation;   // ms, 0 = Windows
            bool keepHiddenForBloom;  // else Bloom's launcher brings it out
            bool keepHiddenOnFlash;   // else a flashing button brings it out
            bool operator==(const Autohide&) const = default;
        } autohide;
        struct Media {
            bool enabled;
            int maxWidth;
            bool operator==(const Media&) const = default;
        } media;
        struct Usage {
            bool enabled;
            LogoColor logoColor;
            int gapBefore, gapAfter;  // px, beside the meter in the tray
            bool sessions;            // the details list Claude Code and Codex sessions
            bool operator==(const Usage&) const = default;
        } usage;
        struct Island {
            bool enabled;
            bool white;  // else black
            bool showClock, showMedia, showUsage, showMemory, showCpu;
            // Which island each item is on: the middle one, or the one in the left
            // or right corner.
            Slot clockAt, mediaAt, usageAt, memoryAt, cpuAt;
            int height;  // px at 100% scale
            int width;   // body, px at 100% scale; 0 = fits the content
            bool clickToReveal;     // else the pointer entering it brings the taskbar out
            int hoverTransparency;  // %, while the pointer rests on it
            bool operator==(const Island&) const = default;
        } island;
    struct Folders {
        bool enabled;
        std::wstring path;  // as typed; folders expands it
        int mergeDelay;     // ms
        bool showLabels;
        int iconSize, appSpacing, popupGap, cornerRadius;  // px
        int cornerSmoothing, popupScale;                   // %
        FolderSide popupSide;
        bool autoRunning;
        std::wstring runningName;
        bool operator==(const Folders&) const = default;
    } folders;
};

Settings g;

int Clamped(PCWSTR name, int lo, int hi) {
    return std::clamp(Wh_GetIntSetting(name), lo, hi);
}

// The index of the setting's value in `options`, 0 if it's none of them.
template <size_t N>
int Choice(PCWSTR name, const PCWSTR (&options)[N]) {
    PCWSTR value = Wh_GetStringSetting(name);
    int index = 0;
    for (size_t i = 0; i < N; i++) {
        if (wcscmp(value, options[i]) == 0) {
            index = (int)i;
        }
    }
    Wh_FreeStringSetting(value);
    return index;
}

std::wstring Text(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring text = value;
    Wh_FreeStringSetting(value);
    return text;
}

void Load() {
    g.pill.enabled = Wh_GetIntSetting(L"pill.enabled");
    g.pill.edge = (Edge)Choice(L"pill.edge", {L"windows", L"top", L"bottom"});
    g.pill.align = (Align)Choice(L"pill.align", {L"center", L"left", L"right"});
    g.pill.style = (Style)Choice(L"pill.style", {L"bloom", L"glass", L"windowGlass"});
    g.pill.theme = (Theme)Choice(L"pill.theme", {L"system", L"dark", L"light"});
    g.pill.glass = Wh_GetIntSetting(L"pill.glass");
    g.pill.cornerRadius = Clamped(L"pill.cornerRadius", 0, 30);
    g.pill.blur = (float)Clamped(L"pill.blur", 0, 30);
    g.pill.tintOpacity = Clamped(L"pill.tintOpacity", 0, 100);

    auto& t = g.tray;
    t.hideVolume = Wh_GetIntSetting(L"tray.hideVolume");
    t.hideNetwork = Wh_GetIntSetting(L"tray.hideNetwork");
    t.hideBattery = Wh_GetIntSetting(L"tray.hideBattery");
    t.hideMicrophone = Wh_GetIntSetting(L"tray.hideMicrophone");
    t.hideLocation = Wh_GetIntSetting(L"tray.hideLocation");
    t.hideStudioEffects = Wh_GetIntSetting(L"tray.hideStudioEffects");
    t.hideRecall = Wh_GetIntSetting(L"tray.hideRecall");
    t.hideLanguageBar = Wh_GetIntSetting(L"tray.hideLanguageBar");
    t.hideLanguageExtras = Wh_GetIntSetting(L"tray.hideLanguageExtras");
    t.hideCopilot = Wh_GetIntSetting(L"tray.hideCopilot");
    t.bell = (Bell)Choice(L"tray.bell", {L"shown", L"whenNew", L"hidden"});
    t.clock = (Clock)Choice(L"tray.clock", {L"timeDate", L"time", L"hidden"});
    t.clock12 = Choice(L"tray.clockFormat", {L"h24", L"h12"}) == 1;
    t.panelsNearTray = Wh_GetIntSetting(L"tray.panelsNearTray");
    t.centerStill = Wh_GetIntSetting(L"tray.centerStill");

    g.size.enabled = Wh_GetIntSetting(L"size.enabled");
    g.size.taskbarHeight = Clamped(L"size.taskbarHeight", 6, 600);
    g.size.iconSize = Clamped(L"size.iconSize", 8, 128);
    g.size.iconSpacing = Clamped(L"size.iconSpacing", 0, 128);

    g.start.mode =
        (StartMode)Choice(L"start.mode", {L"windows", L"hidden", L"bloom"});

    g.autohide.enabled = Wh_GetIntSetting(L"autohide.enabled");
    g.autohide.revealZone = Clamped(L"autohide.revealZone", 1, 100);
    g.autohide.showDelay = Clamped(L"autohide.showDelay", -1, 5000);
    g.autohide.hideDelay = Clamped(L"autohide.hideDelay", -1, 5000);
    g.autohide.animation = Clamped(L"autohide.animation", 0, 2000);
    g.autohide.keepHiddenForBloom = Wh_GetIntSetting(L"autohide.keepHiddenForBloom");
    g.autohide.keepHiddenOnFlash = Wh_GetIntSetting(L"autohide.keepHiddenOnFlash");

    g.media.enabled = Wh_GetIntSetting(L"media.enabled");
    g.media.maxWidth = Clamped(L"media.maxWidth", 40, 600);

    g.usage.enabled = Wh_GetIntSetting(L"usage.enabled");
    g.usage.logoColor = (LogoColor)Choice(L"usage.logoColor", {L"mono", L"brand"});
    g.usage.gapBefore = Clamped(L"usage.gapBefore", 0, 100);
    g.usage.gapAfter = Clamped(L"usage.gapAfter", 0, 100);
    g.usage.sessions = Wh_GetIntSetting(L"usage.sessions");

    g.island.enabled = Wh_GetIntSetting(L"island.enabled");
    g.island.white = Choice(L"island.color", {L"black", L"white"}) == 1;
    g.island.showClock = Wh_GetIntSetting(L"island.showClock");
    g.island.showMedia = Wh_GetIntSetting(L"island.showMedia");
    g.island.showUsage = Wh_GetIntSetting(L"island.showUsage");
    g.island.showMemory = Wh_GetIntSetting(L"island.showMemory");
    g.island.showCpu = Wh_GetIntSetting(L"island.showCpu");
    auto slot = [](PCWSTR name) { return (Slot)Choice(name, {L"center", L"left", L"right"}); };
    g.island.clockAt = slot(L"island.clockAt");
    g.island.mediaAt = slot(L"island.mediaAt");
    g.island.usageAt = slot(L"island.usageAt");
    g.island.memoryAt = slot(L"island.memoryAt");
    g.island.cpuAt = slot(L"island.cpuAt");
    g.island.height = Clamped(L"island.height", 20, 60);
    g.island.width = Clamped(L"island.width", 0, 1200);
    g.island.clickToReveal = Choice(L"island.reveal", {L"hover", L"click"}) == 1;
    g.island.hoverTransparency = Clamped(L"island.hoverTransparency", 0, 100);

    auto& f = g.folders;
    f.enabled = Wh_GetIntSetting(L"folders.enabled");
    f.path = Text(L"folders.foldersPath");
    f.mergeDelay = Clamped(L"folders.mergeDelay", 100, 3000);
    f.showLabels = Wh_GetIntSetting(L"folders.showLabels");
    f.iconSize = Clamped(L"folders.iconSize", 16, 96);
    f.appSpacing = Clamped(L"folders.appSpacing", 0, 40);
    f.popupSide =
        (FolderSide)Choice(L"folders.popupSide", {L"auto", L"above", L"below"});
    f.popupGap = Clamped(L"folders.popupGap", 0, 200);
    f.cornerRadius = Clamped(L"folders.cornerRadius", 0, 40);
    f.cornerSmoothing = Clamped(L"folders.cornerSmoothing", 0, 100);
    f.popupScale = Clamped(L"folders.popupScale", 50, 150);
    f.autoRunning = Wh_GetIntSetting(L"folders.autoRunning");
    f.runningName = Text(L"folders.runningName");
}

// The taskbar's theme: the one set (pill.theme, which Bloom sets to its own), else
// Windows' taskbar theme, not the apps' one.
bool Dark() {
    if (g.pill.enabled && g.pill.theme != Theme::kSystem) {
        return g.pill.theme == Theme::kDark;
    }
    DWORD light = 1, size = sizeof(light);
    RegGetValue(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    return !light;
}

// Shape values that are not on the settings page (the WindowGlass look).
namespace theme {
constexpr double kFrameMaxWidth = 1895;
constexpr double kFrameMinWidth = 100;
constexpr Thickness kAppsMargin{10, 2, 0, 2};
constexpr Thickness kAppsPadding{10, 0, 0, 0};
constexpr Thickness kAppsBorder{0.5, 1, 0, 1};
// A taskbar without a tray (secondary monitors) is the whole pill by itself.
constexpr Thickness kSoloMargin{10, 2, 10, 2};
constexpr Thickness kSoloPadding{10, 0, 10, 0};
constexpr Thickness kSoloBorder{0.5, 1, 0.5, 1};
constexpr Thickness kTrayMargin{0, 2, 0, 2};
// The pill's inner end matches its inner start (kAppsPadding).
constexpr Thickness kTrayPadding{0, 0, 10, 0};
constexpr Thickness kTrayBorder{0, 1, 0.5, 1};
constexpr double kTaskButtonRadius = 10;
constexpr double kTrayButtonRadius = 8;
constexpr Thickness kTrayButtonPadding{6, 0, 6, 0};
constexpr Thickness kTrayButtonMargin{2, 0, 0, 0};
constexpr Thickness kBellPadding{2, 4, 2, 4};
constexpr double kTextIconPadding = 10;
constexpr double kTextIconRadius = 10;
constexpr double kGripperMinWidth = 24;
constexpr double kClockFontSize = 13;
constexpr PCWSTR kClockFont = L"vivo Sans EN VF";
constexpr PCWSTR kTintResource = L"SystemChromeMediumColor";
// The Bloom style's outline: one physical pixel of the theme's text color, faint.
constexpr PCWSTR kEdgeResource = L"SystemBaseHighColor";
constexpr double kEdgeOpacity = 0.08;
// Glass style, top to bottom. Light catches the rim at the top and, refracted
// through the body, again at the bottom; the sides stay dim.
struct GlassStop {
    float offset;
    winrt::Windows::UI::Color color;
};
constexpr GlassStop kGlassRim[] = {{0.0f, {0x38, 0xFF, 0xFF, 0xFF}},
                                   {0.25f, {0x14, 0xFF, 0xFF, 0xFF}},
                                   {0.5f, {0x0A, 0xFF, 0xFF, 0xFF}},
                                   {0.78f, {0x10, 0xFF, 0xFF, 0xFF}},
                                   {1.0f, {0x24, 0xFF, 0xFF, 0xFF}}};
constexpr GlassStop kGlassSheen[] = {{0.0f, {0x3C, 0xFF, 0xFF, 0xFF}},
                                     {0.3f, {0x0C, 0xFF, 0xFF, 0xFF}},
                                     {0.55f, {0x00, 0xFF, 0xFF, 0xFF}},
                                     {0.85f, {0x06, 0xFF, 0xFF, 0xFF}},
                                     {1.0f, {0x2C, 0xFF, 0xFF, 0xFF}}};
constexpr float kGlassSaturation = 1.35f;
// Small taskbar buttons (the taskbar's own posture when it's full).
constexpr int kIconSizeSmall = 20;
constexpr int kButtonWidthSmall = 30;
// Room above and below a task icon inside the bar, for the button's highlight.
constexpr int kIconRoom = 3;
// Media widget, sized for the 55 px taskbar.
constexpr int kMediaArtSize = 32;
constexpr int kMediaArtRadius = kMediaArtSize / 2;
constexpr int kMediaTitleSize = 12;
constexpr int kMediaButtonWidth = 26;
constexpr int kMediaButtonHeight = 28;
constexpr int kMediaGlyphSize = 12;
constexpr PCWSTR kMediaHover = L"#FF7A9E";
// Usage meter colors, dark / light: the text and
// its quieter steps, the bar's track, and amber and red past the warning and
// danger lines (red text is lighter than the red bar).
struct UsagePalette {
    winrt::Windows::UI::Color text, muted, faint, track, warn, danger, dangerText;
};
constexpr UsagePalette kUsageDark{
    {255, 0xEC, 0xED, 0xEE}, {255, 0xA1, 0xA1, 0xAA}, {0x99, 0xEC, 0xED, 0xEE},
    {0x59, 0x7F, 0x7F, 0x7F}, {255, 0xF7, 0xB7, 0x50}, {255, 0xE5, 0x48, 0x4D},
    {255, 0xE7, 0x75, 0x78}};
constexpr UsagePalette kUsageLight{
    {255, 0x11, 0x18, 0x1C}, {255, 0x3F, 0x3F, 0x46}, {255, 0x52, 0x52, 0x5B},
    {0x59, 0x7F, 0x7F, 0x7F}, {255, 0x62, 0x42, 0x0E}, {255, 0xDC, 0x3E, 0x42},
    {255, 0xB3, 0x36, 0x3A}};
// Below the warning line the bar is filled flat in the text color at this alpha.
constexpr uint8_t kUsageFillAlpha = 0xD9;
constexpr double kUsageWarnPercent = 80;
constexpr double kUsageDangerPercent = 90;
// An account: its icon or letter at the left, the number at the right, and under them a
// bar as wide.
constexpr double kUsageBlockWidth = 52;
constexpr double kUsageRowHeight = 16;
constexpr double kUsageLineGap = 3;
constexpr double kUsageBarHeight = 3;
constexpr double kUsageBarRadius = 1;
constexpr double kUsageLogoSize = 14;
constexpr double kUsageInitialSize = 11;  // an account without an icon shows its letter
constexpr double kUsagePercentSize = 12;
constexpr PCWSTR kUsageUnit = L"\u2009%";
constexpr PCWSTR kUsageFont = L"Noto Sans";
constexpr double kUsageDimOpacity = 0.45;
constexpr double kUsageGap = 12;
// The name tag after the logo: as long as this many of its letters at most,
// then cut with an ellipsis; this far from the logo and from the number.
constexpr double kUsageTagMaxEm = 4.5;
constexpr double kUsageTagGap = 4;
// A logo that can't be drawn: its letter on a tile of the text color at this
// alpha, the letter this much of the tile, its corners this much of it.
constexpr uint8_t kUsageTileAlpha = 0x26;
constexpr double kUsageTileLetter = 0.6;
constexpr double kUsageTileRadius = 0.3;
// The Bloom button: Bloom's flower, its petals in the taskbar's ink and its
// middle in Bloom's rose.
constexpr winrt::Windows::UI::Color kBloomRoseDark{255, 0xFF, 0x7A, 0x9E};
constexpr winrt::Windows::UI::Color kBloomRoseLight{255, 0xE0, 0x4E, 0x78};
constexpr double kBloomIconFallback = 24;  // the stock icon's size, before it has one
// Island: sizes in DIPs, or times its height.
constexpr int kIslandFlare = 8;           // the fillets into the screen edge
constexpr double kIslandPadding = 0.4;    // body sides, times the height
constexpr double kIslandTextSize = 0.43;  // times the height
constexpr double kIslandArtSize = 0.7;    // times the height
constexpr double kIslandSystemSize = 0.6;  // memory and CPU icons, times the height
constexpr double kIslandGap = 10;
constexpr double kIslandArtGap = 6;
constexpr double kIslandMeterGap = 4;   // a usage logo to its number
constexpr double kIslandUsageLead = 8;  // more before the usage, a group of its own
constexpr double kIslandMinWidth = 64;
constexpr double kIslandApart = 4;  // the middle island kept this far from a corner one
constexpr int kIslandFadeMs = 150;
constexpr int kIslandFadeDelayMs = 200;  // waits out the taskbar's slide
// The hover transparency comes in slowly and goes a little quicker, eased at
// both ends (sine in-out), so it reads as a fade and not a switch.
constexpr int kIslandHoverInMs = 300;
constexpr int kIslandHoverOutMs = 220;
constexpr float kIslandHoverEase[4] = {0.37f, 0.0f, 0.63f, 1.0f};
constexpr PCWSTR kIslandFallbackFont = L"Segoe UI Variable Text";
// Black in both themes, so the text is always light.
constexpr winrt::Windows::UI::Color kIslandFill{0xFF, 0x00, 0x00, 0x00};
constexpr winrt::Windows::UI::Color kIslandText{0xFF, 0xFF, 0xFF, 0xFF};
constexpr winrt::Windows::UI::Color kIslandDimText{0xB8, 0xFF, 0xFF, 0xFF};
// island.color white: the light theme's text on white.
constexpr winrt::Windows::UI::Color kIslandWhiteFill{0xFF, 0xFF, 0xFF, 0xFF};
constexpr winrt::Windows::UI::Color kIslandWhiteText{0xFF, 0x11, 0x18, 0x1C};
constexpr winrt::Windows::UI::Color kIslandWhiteDimText{0xB8, 0x11, 0x18, 0x1C};
// The morph between the island and the pill. Its length is the taskbar's
// slide too, unless the auto-hide animation length is set.
constexpr int kIslandMorphMs = 280;
// Along the morph: the island's content is gone by here, and from here on the
// shape fades onto the glass pill.
constexpr float kIslandContentOut = 0.3f;
constexpr float kIslandShapeFade = 0.5f;
// The glass pill's look at a glance, which the shape's color turns into:
// SystemChromeMediumColor, dark / light, at the pill's tint opacity.
constexpr winrt::Windows::UI::Color kIslandPillDark{255, 0x1F, 0x1F, 0x1F};
constexpr winrt::Windows::UI::Color kIslandPillLight{255, 0xE6, 0xE6, 0xE6};
// Ease-out (the Fluent decelerate curve).
constexpr float kIslandEase[4] = {0.1f, 0.9f, 0.2f, 1.0f};
}  // namespace theme

// Where the pill sits along the edge: only the pill shape moves it off the middle.
Align PillAlign() {
    return g.pill.enabled ? g.pill.align : Align::kCenter;
}

// The tray piece's margin. A pill set against the right side keeps as far off it
// as its apps piece keeps off the left one (kAppsMargin).
Thickness TrayMargin() {
    Thickness m = theme::kTrayMargin;
    if (PillAlign() == Align::kRight) {
        m.Right = theme::kAppsMargin.Left;
    }
    return m;
}

}  // namespace settings

// ============================================================================
namespace core {
// ============================================================================

std::atomic<bool> g_unloading;

// ---- XAML tree --------------------------------------------------------------

FrameworkElement Parent(FrameworkElement const& e) {
    return Media::VisualTreeHelper::GetParent(e).try_as<FrameworkElement>();
}

bool IsClass(FrameworkElement const& e, PCWSTR className) {
    return winrt::get_class_name(e) == className;
}

template <class F>
FrameworkElement FindChild(FrameworkElement const& e, F&& pred) {
    int n = Media::VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (child && pred(child)) {
            return child;
        }
    }
    return nullptr;
}

FrameworkElement ChildNamed(FrameworkElement const& e, PCWSTR name) {
    return e ? FindChild(e, [&](auto& c) { return c.Name() == name; })
             : nullptr;
}

FrameworkElement ChildOfClass(FrameworkElement const& e, PCWSTR className) {
    return e ? FindChild(e, [&](auto& c) { return IsClass(c, className); })
             : nullptr;
}

// Depth-first; `visit` returns true to stop.
template <class F>
FrameworkElement FindDescendant(FrameworkElement const& e, F&& visit, int depth) {
    if (!e || depth <= 0) {
        return nullptr;
    }
    int n = Media::VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (visit(child)) {
            return child;
        }
        if (auto found = FindDescendant(child, visit, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

template <class F>
FrameworkElement FindAncestor(FrameworkElement const& e, F&& pred) {
    for (auto p = e ? Parent(e) : nullptr; p; p = Parent(p)) {
        if (pred(p)) {
            return p;
        }
    }
    return nullptr;
}

FrameworkElement AncestorNamed(FrameworkElement const& e, PCWSTR name) {
    return FindAncestor(e, [&](auto& p) { return p.Name() == name; });
}

FrameworkElement FromAbi(void* unknown) {
    FrameworkElement e = nullptr;
    if (unknown) {
        ((IUnknown*)unknown)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(e));
    }
    return e;
}

// Diagnostics go to %TEMP%\pilltaskbar-diag.txt, bounded: past kDiagCap bytes
// the file becomes pilltaskbar-diag.old.txt and starts over, a line equal to
// the one before is only counted, and a file that won't open is left alone for
// kDiagRetryMs. Any thread.
constexpr ULONGLONG kDiagCap = 256 << 10;
constexpr DWORD kDiagRetryMs = 60000;

ULONGLONG FileSize(PCWSTR path) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExW(path, GetFileExInfoStandard, &data)) {
        return 0;
    }
    return ((ULONGLONG)data.nFileSizeHigh << 32) | data.nFileSizeLow;
}

void DiagLine(PCWSTR format, ...) {
    WCHAR line[512];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(line, _TRUNCATE, format, args);
    va_end(args);

    static std::mutex lock;
    static WCHAR path[MAX_PATH], old[MAX_PATH], last[ARRAYSIZE(line)];
    static ULONG repeats;
    static ULONGLONG size = ~0ull;  // written so far as counted here; unknown at first
    static DWORD failedAt;          // tick of the open that failed, 0 for none
    std::lock_guard guard(lock);
    if (wcscmp(line, last) == 0) {
        repeats++;
        return;
    }
    if (failedAt && GetTickCount() - failedAt < kDiagRetryMs) {
        return;
    }
    failedAt = 0;
    if (!path[0]) {
        DWORD n = GetTempPath(MAX_PATH, path);
        if (!n || n > MAX_PATH - 32) {
            path[0] = 0;
            failedAt = GetTickCount() | 1;
            return;
        }
        wcscpy_s(old, path);
        wcscat_s(path, L"pilltaskbar-diag.txt");
        wcscat_s(old, L"pilltaskbar-diag.old.txt");
    }
    // Past the cap by the count, the file's own size decides: it may have been
    // deleted, or another explorer writes to it too.
    if (size > kDiagCap) {
        size = FileSize(path);
    }
    bool restart = false;  // the file couldn't be moved aside: it's emptied
    if (size > kDiagCap) {
        restart = !MoveFileExW(path, old, MOVEFILE_REPLACE_EXISTING);
        size = 0;
    }
    FILE* f = _wfopen(path, restart ? L"w, ccs=UTF-8" : L"a, ccs=UTF-8");
    if (!f) {
        failedAt = GetTickCount() | 1;
        return;
    }
    SYSTEMTIME t;
    GetLocalTime(&t);
    int written = 0;
    if (repeats) {
        written += fwprintf(f, L"%02d:%02d:%02d.%03d (the line above %lu more times)\n",
                            t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, repeats);
    }
    written += fwprintf(f, L"%02d:%02d:%02d.%03d %s\n", t.wHour, t.wMinute, t.wSecond,
                        t.wMilliseconds, line);
    fclose(f);
    size += written > 0 ? written + 2 : 0;  // about: CRLF, mostly ASCII
    repeats = 0;
    wcscpy_s(last, line);
}

// Exceptions must not reach XAML or the explorer code around a hook: XAML turns
// them into a crash.
template <class F>
void Guard(PCWSTR what, F&& work) {
    try {
        work();
    } catch (...) {
        Wh_Log(L"%s failed: %08X", what, winrt::to_hresult());
        DiagLine(L"%s failed: %08X", what, winrt::to_hresult());
    }
}

// ---- restore list -----------------------------------------------------------
// Every property the mod writes goes through Set, which keeps the value it
// replaced; RestoreAll puts them all back. Only touched on the taskbar thread.

struct Saved {
    winrt::weak_ref<DependencyObject> obj;
    DependencyProperty prop{nullptr};
    wf::IInspectable value{nullptr};
    bool templateBound = false;
    PCWSTR bindName = nullptr;
};

std::vector<Saved> g_saved;
size_t g_pruneAt = 64;

struct SavedColumns {
    winrt::weak_ref<Controls::Grid> grid;
    std::vector<Controls::ColumnDefinition> columns;
};

std::vector<SavedColumns> g_savedColumns;

bool SameValue(wf::IInspectable const& a, wf::IInspectable const& b) {
    if (a == b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    if (auto x = a.try_as<wf::IReference<Thickness>>()) {
        auto y = b.try_as<wf::IReference<Thickness>>();
        return y && x.Value() == y.Value();
    }
    if (auto x = a.try_as<wf::IReference<CornerRadius>>()) {
        auto y = b.try_as<wf::IReference<CornerRadius>>();
        return y && x.Value() == y.Value();
    }
    if (auto x = a.try_as<wf::IReference<double>>()) {
        auto y = b.try_as<wf::IReference<double>>();
        // NaN is a size's "Auto"; two of them are the same value.
        return y && (x.Value() == y.Value() ||
                     (std::isnan(x.Value()) && std::isnan(y.Value())));
    }
    if (auto x = a.try_as<wf::IReference<Visibility>>()) {
        auto y = b.try_as<wf::IReference<Visibility>>();
        return y && x.Value() == y.Value();
    }
    return false;
}

// `bindName`: the property's name, for values a template binds to the
// templated parent's property of the same name. A TemplateBinding can't be set
// again, so restoring binds it to that property, if that gives the old value.
void Set(DependencyObject const& obj,
         DependencyProperty const& prop,
         wf::IInspectable const& value,
         PCWSTR bindName = nullptr) {
    bool known = std::any_of(g_saved.begin(), g_saved.end(), [&](auto& s) {
        return s.prop == prop && s.obj.get() == obj;
    });
    if (!known) {
        if (g_saved.size() >= g_pruneAt) {
            std::erase_if(g_saved, [](auto& s) { return !s.obj.get(); });
            g_pruneAt = std::max<size_t>(64, g_saved.size() * 2);
        }
        Saved s{winrt::make_weak(obj), prop};
        auto local = obj.ReadLocalValue(prop);
        s.templateBound = !local || local.try_as<Data::BindingExpressionBase>();
        s.value = s.templateBound ? obj.GetAnimationBaseValue(prop) : local;
        s.bindName = bindName;
        g_saved.push_back(std::move(s));
    }
    obj.SetValue(prop, value);
}

bool Rebind(FrameworkElement const& e, Saved const& s) {
    Data::RelativeSource source;
    source.Mode(Data::RelativeSourceMode::TemplatedParent);
    Data::Binding binding;
    binding.RelativeSource(source);
    binding.Path(PropertyPath(s.bindName));
    e.SetBinding(s.prop, binding);
    return SameValue(e.GetAnimationBaseValue(s.prop), s.value);
}

// Replaces a grid's columns, keeping the original definitions for RestoreAll.
void SetColumns(Controls::Grid const& grid,
                std::initializer_list<GridLength> widths) {
    bool known =
        std::any_of(g_savedColumns.begin(), g_savedColumns.end(),
                    [&](auto& s) { return s.grid.get() == grid; });
    auto columns = grid.ColumnDefinitions();
    if (!known) {
        SavedColumns saved{winrt::make_weak(grid)};
        for (auto const& c : columns) {
            saved.columns.push_back(c);
        }
        g_savedColumns.push_back(std::move(saved));
    }
    columns.Clear();
    for (auto w : widths) {
        Controls::ColumnDefinition c;
        c.Width(w);
        columns.Append(c);
    }
}

// Property change callbacks of the mod, one per object, property and user
// (`who`: two parts that watch the same property each name themselves);
// RestoreAll removes them.
struct Watched {
    winrt::weak_ref<DependencyObject> obj;
    DependencyProperty prop{nullptr};
    int64_t token;
    PCWSTR who;
};

std::vector<Watched> g_watched;
size_t g_watchPruneAt = 64;

template <class F>
void Watch(DependencyObject const& obj,
           DependencyProperty const& prop,
           F&& onChange,
           PCWSTR who = L"") {
    bool known = std::any_of(g_watched.begin(), g_watched.end(), [&](auto& w) {
        return w.prop == prop && w.obj.get() == obj && wcscmp(w.who, who) == 0;
    });
    if (known) {
        return;
    }
    if (g_watched.size() >= g_watchPruneAt) {
        std::erase_if(g_watched, [](auto& w) { return !w.obj.get(); });
        g_watchPruneAt = std::max<size_t>(64, g_watched.size() * 2);
    }
    auto token = obj.RegisterPropertyChangedCallback(
        prop, [onChange = std::forward<F>(onChange)](
                  DependencyObject const& sender, DependencyProperty const&) {
            if (!g_unloading) {
                Guard(L"Watch", [&] { onChange(sender); });
            }
        });
    g_watched.push_back({winrt::make_weak(obj), prop, token, who});
}

// For properties explorer writes again later: Set, then put the value back
// whenever it changes.
void Keep(DependencyObject const& obj,
          DependencyProperty const& prop,
          wf::IInspectable const& value) {
    Set(obj, prop, value);
    Watch(obj, prop, [prop, value](DependencyObject const& sender) {
        if (!SameValue(sender.GetValue(prop), value)) {
            sender.SetValue(prop, value);
        }
    });
}

void RestoreAll() {
    for (auto& w : g_watched) {
        if (auto obj = w.obj.get()) {
            Guard(L"Unwatch",
                  [&] { obj.UnregisterPropertyChangedCallback(w.prop, w.token); });
        }
    }
    g_watched.clear();
    g_watchPruneAt = 64;

    for (auto it = g_saved.rbegin(); it != g_saved.rend(); ++it) {
        auto obj = it->obj.get();
        if (!obj) {
            continue;
        }
        try {
            if (it->value == DependencyProperty::UnsetValue()) {
                obj.ClearValue(it->prop);
                continue;
            }
            auto e = obj.try_as<FrameworkElement>();
            if (it->templateBound && it->bindName && e && Rebind(e, *it)) {
                continue;
            }
            obj.SetValue(it->prop, it->value);
        } catch (...) {
            Wh_Log(L"Restore failed: %08X", winrt::to_hresult());
        }
    }
    g_saved.clear();
    g_pruneAt = 64;

    for (auto& s : g_savedColumns) {
        if (auto grid = s.grid.get()) {
            auto columns = grid.ColumnDefinitions();
            columns.Clear();
            for (auto& c : s.columns) {
                columns.Append(c);
            }
        }
    }
    g_savedColumns.clear();
}

// ---- taskbar windows ----------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;
using GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;
void* TaskbarHost_FrameHeight_Original;
using Decref_t = void(WINAPI*)(void* pThis);
Decref_t std__Ref_count_base__Decref_Original;

bool IsSecondary(HWND hwnd) {
    WCHAR cls[32];
    return GetClassName(hwnd, cls, ARRAYSIZE(cls)) &&
           _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[32];
    DWORD pid = 0;
    return GetWindowThreadProcessId(hwnd, &pid) &&
           pid == GetCurrentProcessId() &&
           GetClassName(hwnd, cls, ARRAYSIZE(cls)) &&
           (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

std::vector<HWND> Taskbars() {
    std::vector<HWND> list;
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            if (IsTaskbarWindow(hwnd)) {
                auto& list = *(std::vector<HWND>*)param;
                // The primary first: callers that want one taskbar take it.
                if (IsSecondary(hwnd)) {
                    list.push_back(hwnd);
                } else {
                    list.insert(list.begin(), hwnd);
                }
            }
            return TRUE;
        },
        (LPARAM)&list);
    return list;
}

HWND PrimaryTaskbar() {
    auto list = Taskbars();
    return list.empty() || IsSecondary(list[0]) ? nullptr : list[0];
}

// The monitor a taskbar is on. Not MonitorFromWindow: a hidden top taskbar's
// window is mostly on the monitor above, if there is one.
HMONITOR MonitorOf(HWND taskbar) {
    if (auto monitor = (HMONITOR)GetProp(taskbar, L"TaskbarMonitor")) {
        return monitor;
    }
    return MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
}

// The XAML element the taskbar window hosts, through CTaskBand::GetTaskbarHost.
// Call on the taskbar thread.
FrameworkElement TaskbarElement(HWND hwnd) {
    bool secondary = IsSecondary(hwnd);
    void* vftable = secondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                              : CTaskBand_ITaskListWndSite_vftable;
    auto getHost = secondary ? CSecondaryTaskBand_GetTaskbarHost_Original
                             : CTaskBand_GetTaskbarHost_Original;
    if (!vftable || !getHost || !TaskbarHost_FrameHeight_Original) {
        return nullptr;
    }

    HWND band = secondary ? FindWindowEx(hwnd, nullptr, L"WorkerW", nullptr)
                          : (HWND)GetProp(hwnd, L"TaskbandHWND");
    void* site = band ? (void*)GetWindowLongPtr(band, 0) : nullptr;
    for (int i = 0; site && *(void**)site != vftable; i++) {
        if (i == 20) {
            return nullptr;
        }
        site = (void**)site + 1;
    }
    if (!site) {
        return nullptr;
    }

    void* host[2]{};  // std::shared_ptr<TaskbarHost>
    getHost(site, host);
    if (!host[0]) {
        if (host[1]) {
            std__Ref_count_base__Decref_Original(host[1]);
        }
        return nullptr;
    }

    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48   <- offset of the element in TaskbarHost
    size_t offset = 0x10;
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        offset = b[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
    }

    auto element = FromAbi(*(IUnknown**)((BYTE*)host[0] + offset));
    std__Ref_count_base__Decref_Original(host[1]);
    return element;
}

XamlRoot TaskbarRoot(HWND hwnd) {
    auto e = TaskbarElement(hwnd);
    return e ? e.XamlRoot() : nullptr;
}

HWND HwndOf(XamlRoot const& root) {
    for (HWND hwnd : Taskbars()) {
        if (TaskbarRoot(hwnd) == root) {
            return hwnd;
        }
    }
    return nullptr;
}

// Vertical (left/right) taskbars keep the stock look: every shape here is
// horizontal. Set once per apply pass.
bool g_vertical;

// The same test on every path, so the apply pass and the move check agree.
bool LooksVertical(HWND hwnd) {
    RECT rc;
    return hwnd && GetWindowRect(hwnd, &rc) &&
           rc.bottom - rc.top > rc.right - rc.left;
}

// ---- taskbar thread -----------------------------------------------------------
// Each taskbar window gets one subclass: it runs work sent from other threads
// (RunOn) and tells features when the window moves.

[[clang::no_destroy]] winrt::Windows::System::DispatcherQueue g_queue{nullptr};
std::mutex g_lock;  // g_queue, g_attached
std::vector<HWND> g_attached;
UINT RunMsg() {
    static UINT msg = RegisterWindowMessage(L"PillTaskbar_Run_" WH_MOD_ID);
    return msg;
}
thread_local bool g_ownWindowPosChange;
void (*g_onWindowMoved)(HWND) = nullptr;
// Any move of a taskbar window, ours included (auto-hide state).
void (*g_onTaskbarMoved)(HWND) = nullptr;
// A taskbar window changed its place in the z-order (the island keeps above it
// while it morphs).
void (*g_onTaskbarRestacked)(HWND) = nullptr;
// True while a taskbar window has to stay in the topmost band: a restack that
// would take it out of there keeps its place instead.
bool (*g_keepsTaskbarTopmost)(HWND) = nullptr;
// A taskbar window is going, e.g. with its monitor.
void (*g_onTaskbarDestroyed)(HWND) = nullptr;
// The primary taskbar turned vertical or back (F5); the shape features step
// aside or come back.
void (*g_onOrientationChanged)() = nullptr;
// True to drop a tick of explorer's own timer `id` on a taskbar window (the
// tray's hide check while auto-hide keeps the taskbar out).
bool (*g_dropsTimer)(HWND, UINT_PTR id) = nullptr;
// The same for the folders, whose popup keeps its taskbar out under it.
bool (*g_popupDropsTimer)(HWND, UINT_PTR id) = nullptr;
// The screen edge (ABE_*) a taskbar setting change has to move the taskbar to
// instead of the one it names, or -1 to leave it (F11).
int (*g_forcedEdge)() = nullptr;
// That change: a taskbar setting (wParam), the location being 6, now set to
// lParam. The Settings app sends it after storing the location.
constexpr UINT kSettingChangeMsg = 0x5CA;
constexpr WPARAM kLocationSetting = 6;
// The memory add-on's readings on the primary taskbar window changed
// (docs/addon-protocol.md); the island shows them.
void (*g_onFiguresChanged)() = nullptr;
UINT FiguresChangedMsg() {
    static UINT msg = RegisterWindowMessage(L"MemoryClean.FiguresChanged");
    return msg;
}



void NoteUiThread() {
    if (g_queue) {
        return;
    }
    if (auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread()) {
        std::lock_guard guard(g_lock);
        g_queue = queue;
    }
}

// Diagnostics: the last hit test a secondary taskbar got, and how it came.
struct HitTest {
    HWND hwnd;
    DWORD tick;
    POINT at, cursor;
    DWORD send;  // InSendMessageEx
};
HitTest g_lastHitTest;

LRESULT CALLBACK TaskbarSubclass(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam,
                                 DWORD_PTR) {
    if (msg == RunMsg() && lParam) {
        try {
            (*(std::function<void()>*)lParam)();
        } catch (...) {
            Wh_Log(L"Taskbar work failed: %08X", winrt::to_hresult());
        }
        return 0;
    }
    if (msg == FiguresChangedMsg()) {
        if (g_onFiguresChanged) {
            Guard(L"Figures changed", [] { g_onFiguresChanged(); });
        }
        return 0;
    }
    switch (msg) {
        case kSettingChangeMsg:
            if (wParam == kLocationSetting && g_forcedEdge) {
                int edge = -1;
                Guard(L"Taskbar location", [&] { edge = g_forcedEdge(); });
                if (edge >= 0) {
                    lParam = edge;
                }
            }
            break;
        case WM_NCHITTEST:
            if (IsSecondary(hwnd)) {
                g_lastHitTest = {hwnd, GetTickCount(),
                                 {(short)LOWORD(lParam), (short)HIWORD(lParam)}, {},
                                 InSendMessageEx(nullptr)};
                GetCursorPos(&g_lastHitTest.cursor);
            }
            break;
        case WM_TIMER: {
            // Only a timer without a procedure of its own comes here.
            bool drop = false;
            if (!lParam && g_dropsTimer) {
                Guard(L"Taskbar timer", [&] { drop = g_dropsTimer(hwnd, wParam); });
            }
            if (!lParam && !drop && g_popupDropsTimer) {
                Guard(L"Taskbar timer", [&] { drop = g_popupDropsTimer(hwnd, wParam); });
            }
            if (drop) {
                return 0;
            }
            break;
        }
        case WM_WINDOWPOSCHANGING: {
            // Out of the topmost band: before HWND_NOTOPMOST or HWND_BOTTOM, or
            // under a window that isn't topmost itself.
            auto* pos = (WINDOWPOS*)lParam;
            bool keep = false;
            if (g_keepsTaskbarTopmost && pos && !(pos->flags & SWP_NOZORDER)) {
                HWND after = pos->hwndInsertAfter;
                bool leaves = after == HWND_NOTOPMOST || after == HWND_BOTTOM ||
                              (after != HWND_TOP && after != HWND_TOPMOST &&
                               !(GetWindowLongPtrW(after, GWL_EXSTYLE) & WS_EX_TOPMOST));
                if (leaves) {
                    Guard(L"Taskbar keeps topmost", [&] { keep = g_keepsTaskbarTopmost(hwnd); });
                }
            }
            if (keep) {
                pos->flags |= SWP_NOZORDER;
            }
            break;
        }
        case WM_WINDOWPOSCHANGED: {
            LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
            if (g_onWindowMoved && !g_ownWindowPosChange) {
                Guard(L"Window moved", [&] { g_onWindowMoved(hwnd); });
            }
            // Only a real move or resize: a region change comes through here
            // too, and must not stand for a slide that hasn't happened yet.
            auto* pos = (WINDOWPOS*)lParam;
            constexpr UINT kStill = SWP_NOMOVE | SWP_NOSIZE;
            if (g_onTaskbarMoved && (!pos || (pos->flags & kStill) != kStill)) {
                Guard(L"Taskbar moved", [&] { g_onTaskbarMoved(hwnd); });
            }
            if (g_onTaskbarRestacked && pos && !(pos->flags & SWP_NOZORDER)) {
                Guard(L"Taskbar restacked", [&] { g_onTaskbarRestacked(hwnd); });
            }
            if (g_onOrientationChanged && !IsSecondary(hwnd) &&
                LooksVertical(hwnd) != g_vertical) {
                Guard(L"Orientation", [] { g_onOrientationChanged(); });
            }
            return result;
        }
        case WM_NCDESTROY: {
            {
                std::lock_guard guard(g_lock);
                std::erase(g_attached, hwnd);
            }
            if (g_onTaskbarDestroyed) {
                Guard(L"Taskbar destroyed", [&] { g_onTaskbarDestroyed(hwnd); });
            }
            break;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

bool Attach(HWND hwnd) {
    {
        std::lock_guard guard(g_lock);
        if (std::ranges::find(g_attached, hwnd) != g_attached.end()) {
            return true;
        }
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TaskbarSubclass,
                                                       0)) {
        Wh_Log(L"Subclassing %08X failed", (DWORD)(ULONG_PTR)hwnd);
        return false;
    }
    std::lock_guard guard(g_lock);
    g_attached.push_back(hwnd);
    return true;
}

void DetachAll() {
    std::vector<HWND> list;
    {
        std::lock_guard guard(g_lock);
        list.swap(g_attached);
    }
    for (HWND hwnd : list) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, TaskbarSubclass);
    }
}

// Runs `work` on the window's thread and waits for it.
bool RunOn(HWND hwnd, std::function<void()> work) {
    if (!hwnd || !Attach(hwnd)) {
        return false;
    }
    SendMessage(hwnd, RunMsg(), 0, (LPARAM)&work);
    return true;
}

// Runs `work` on the taskbar thread later, from any thread; false before the
// thread is known or while unloading. Wh_ModUninit waits for queued work.
std::atomic<int> g_posted;

bool Post(std::function<void()> work) {
    if (g_unloading) {
        return false;
    }
    std::lock_guard guard(g_lock);
    if (!g_queue) {
        return false;
    }
    g_posted++;
    bool queued = g_queue.TryEnqueue([work = std::move(work)] {
        if (!g_unloading) {
            Guard(L"Posted work", work);
        }
        g_posted--;
    });
    if (!queued) {
        g_posted--;
    }
    return queued;
}

void WaitForPosted() {
    while (g_posted > 0) {
        Sleep(10);
    }
}

}  // namespace core

namespace autohide {
bool Collapsed(HWND hwnd);
// The primary tray's TrayUI, as TrayUI::_HandleSettingChange gets it; checked
// before any use.
inline std::atomic<void*> g_tray;
}  // namespace autohide

// ============================================================================
namespace region {
// ============================================================================
// Clips the full-width taskbar window to the pill pieces so the empty sides are
// neither drawn nor hit-tested. Recomputed on LayoutUpdated, applied only when
// it changes; reapplied when the window moves, since explorer resets the window
// region across auto-hide transitions.

struct Bar {
    HWND hwnd = nullptr;
    winrt::weak_ref<FrameworkElement> frame;
    FrameworkElement::LayoutUpdated_revoker layoutUpdated;
    RECT applied{};  // bounding box of our region, empty if none
    RECT pieces[2]{};
};

std::list<Bar> g_bars;  // taskbar thread only

HWND ResolveHwnd(Bar& bar, FrameworkElement const& frame) {
    if (!bar.hwnd) {
        if (auto root = frame.XamlRoot()) {
            bar.hwnd = core::HwndOf(root);
            if (bar.hwnd) {
                core::Attach(bar.hwnd);
            }
        }
    }
    return bar.hwnd;
}

void Update(Bar& bar) {
    auto frame = bar.frame.get();
    if (!frame) {
        return;
    }
    HWND hwnd = ResolveHwnd(bar, frame);
    auto root = frame.XamlRoot();
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    if (!hwnd || !content) {
        return;
    }
    // A collapsed top taskbar keeps a thin strip at its bottom edge to bring it
    // back; the pill region would cut that strip away.
    if (autohide::Collapsed(hwnd)) {
        RECT current;
        if (!IsRectEmpty(&bar.applied) && GetWindowRgnBox(hwnd, &current) != ERROR &&
            EqualRect(&current, &bar.applied)) {
            core::g_ownWindowPosChange = true;
            SetWindowRgn(hwnd, nullptr, TRUE);
            core::g_ownWindowPosChange = false;
        }
        bar.applied = {};
        memset(bar.pieces, 0, sizeof(bar.pieces));
        return;
    }

    RECT window;
    if (!GetWindowRect(hwnd, &window)) {
        return;
    }
    int width = window.right - window.left;
    int height = window.bottom - window.top;
    // The island fills the borderless taskbar window, so its DIPs scale to
    // window pixels by the window DPI. RasterizationScale is snapped to standard
    // steps and comes out wrong for custom scaling.
    UINT dpi = GetDpiForWindow(hwnd);
    double scale = dpi ? dpi / 96.0 : root.RasterizationScale();

    auto toWindow = [&](FrameworkElement const& e, RECT& out) {
        if (!e || e.Visibility() != Visibility::Visible) {
            out = {};
            return;
        }
        auto r = e.TransformToVisual(content).TransformBounds(
            {0, 0, (float)e.ActualWidth(), (float)e.ActualHeight()});
        out = {std::clamp((int)std::lround(r.X * scale), 0, width),
               std::clamp((int)std::lround(r.Y * scale), 0, height),
               std::clamp((int)std::lround((r.X + r.Width) * scale), 0, width),
               std::clamp((int)std::lround((r.Y + r.Height) * scale), 0,
                          height)};
    };

    RECT pieces[2];
    toWindow(frame, pieces[0]);
    toWindow(core::ChildOfClass(core::Parent(frame), L"SystemTray.SystemTrayFrame"),
             pieces[1]);
    if (IsRectEmpty(&pieces[0])) {
        return;  // not laid out yet; an empty region would hide the taskbar
    }

    RECT current;
    bool hasRegion = GetWindowRgnBox(hwnd, &current) != ERROR;
    bool ours = hasRegion && EqualRect(&current, &bar.applied);
    // Explorer narrows the window itself while the tablet taskbar is folded;
    // only take over a window that has no region or a full-window one.
    RECT full{0, 0, width, height};
    if (hasRegion && !ours && !EqualRect(&current, &full)) {
        return;
    }
    if (ours && !memcmp(pieces, bar.pieces, sizeof(pieces))) {
        return;
    }

    HRGN rgn = CreateRectRgnIndirect(&pieces[0]);
    if (!rgn) {
        return;
    }
    RECT box = pieces[0];
    if (!IsRectEmpty(&pieces[1])) {
        if (HRGN tray = CreateRectRgnIndirect(&pieces[1])) {
            CombineRgn(rgn, rgn, tray, RGN_OR);
            DeleteObject(tray);
            UnionRect(&box, &box, &pieces[1]);
        }
    }

    // SetWindowRgn sends WM_WINDOWPOSCHANGED, which must not reenter here.
    core::g_ownWindowPosChange = true;
    BOOL set = SetWindowRgn(hwnd, rgn, TRUE);
    core::g_ownWindowPosChange = false;
    if (set) {
        bar.applied = box;
        memcpy(bar.pieces, pieces, sizeof(pieces));
    } else {
        DeleteObject(rgn);
        Wh_Log(L"SetWindowRgn failed");
    }
}

void OnWindowMoved(HWND hwnd) {
    for (auto& bar : g_bars) {
        if (bar.hwnd == hwnd) {
            Update(bar);
        }
    }
}

void Track(FrameworkElement const& frame, HWND hwnd) {
    std::erase_if(g_bars, [](Bar& b) { return !b.frame.get(); });
    for (auto& bar : g_bars) {
        if (bar.frame.get() == frame) {
            Update(bar);
            return;
        }
    }
    auto& bar = g_bars.emplace_back();
    bar.hwnd = hwnd;
    bar.frame = frame;
    Bar* p = &bar;  // std::list keeps it in place
    bar.layoutUpdated = frame.LayoutUpdated(
        winrt::auto_revoke,
        [p](auto&&, auto&&) { core::Guard(L"Region", [&] { Update(*p); }); });
    core::g_onWindowMoved = OnWindowMoved;
    Update(bar);
}

void Clear() {
    core::g_onWindowMoved = nullptr;
    for (auto& bar : g_bars) {
        RECT current;
        if (bar.hwnd && IsWindow(bar.hwnd) &&
            GetWindowRgnBox(bar.hwnd, &current) != ERROR &&
            EqualRect(&current, &bar.applied)) {
            core::g_ownWindowPosChange = true;
            SetWindowRgn(bar.hwnd, nullptr, TRUE);
            core::g_ownWindowPosChange = false;
        }
    }
    g_bars.clear();
}

}  // namespace region

// ============================================================================
namespace pill {
// ============================================================================

namespace theme = settings::theme;
using winrt::box_value;

// ---- blurred background -------------------------------------------------------
// Backdrop blur with a tint on top, as a XAML brush. The tint follows the theme
// through a proxy SolidColorBrush that resolves the theme resource in the
// owner's resources.

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop,
                       IUnknown,
                       "2FC57384-A068-44D7-A331-30982FCF7177") {
    STDMETHOD(GetEffectId)(GUID * id) PURE;
    STDMETHOD(GetNamedPropertyMapping)(LPCWSTR name, UINT * index,
                                       int* mapping) PURE;
    STDMETHOD(GetPropertyCount)(UINT * count) PURE;
    STDMETHOD(GetProperty)(UINT index, void** value) PURE;
    STDMETHOD(GetSource)(UINT index, void** source) PURE;
    STDMETHOD(GetSourceCount)(UINT * count) PURE;
};

}  // namespace pill

template <>
inline constexpr winrt::guid winrt::impl::guid_v<pill::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

namespace pill {

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;

// One D2D effect: its properties are indexed 0..n-1 in the D2D order.
struct Effect : winrt::implements<Effect,
                                  wge::IGraphicsEffect,
                                  wge::IGraphicsEffectSource,
                                  IGraphicsEffectD2D1Interop> {
    // `name` must be unique in the effect graph.
    Effect(PCWSTR name,
           GUID id,
           std::vector<wf::IInspectable> props,
           std::vector<wge::IGraphicsEffectSource> sources)
        : m_id(id),
          m_props(std::move(props)),
          m_sources(std::move(sources)),
          m_name(name) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    STDMETHOD(GetEffectId)(GUID* id) override {
        *id = m_id;
        return S_OK;
    }
    STDMETHOD(GetNamedPropertyMapping)(LPCWSTR, UINT*, int*) override {
        return E_INVALIDARG;
    }
    STDMETHOD(GetPropertyCount)(UINT* count) override {
        *count = (UINT)m_props.size();
        return S_OK;
    }
    STDMETHOD(GetProperty)(UINT index, void** value) override {
        if (index >= m_props.size()) {
            return E_BOUNDS;
        }
        winrt::copy_to_abi(m_props[index].as<wf::IPropertyValue>(), *value);
        return S_OK;
    }
    STDMETHOD(GetSource)(UINT index, void** source) override {
        if (index >= m_sources.size()) {
            return E_BOUNDS;
        }
        winrt::copy_to_abi(m_sources[index], *source);
        return S_OK;
    }
    STDMETHOD(GetSourceCount)(UINT* count) override {
        *count = (UINT)m_sources.size();
        return S_OK;
    }

   private:
    GUID m_id;
    std::vector<wf::IInspectable> m_props;
    std::vector<wge::IGraphicsEffectSource> m_sources;
    winrt::hstring m_name;
};

// D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED and CLSID_D2D1AlphaMask, missing from
// the MinGW headers.
constexpr UINT32 kBlurOptimizationBalanced = 1;
constexpr GUID kAlphaMaskEffect{
    0xC80ECFF0, 0x3FD5, 0x4F05, {0x83, 0x28, 0xC5, 0xD1, 0x72, 0x4B, 0x4F, 0x0A}};

wuc::CompositionGradientBrush Sheen(wuc::Compositor const& compositor) {
    auto sheen = compositor.CreateLinearGradientBrush();
    sheen.StartPoint({0, 0});
    sheen.EndPoint({0, 1});
    auto stops = sheen.ColorStops();
    for (auto [offset, color] : theme::kGlassSheen) {
        stops.Append(compositor.CreateColorGradientStop(offset, color));
    }
    return sheen;
}

// The glass over the "backdrop" source: a blur if `blur` is set, saturation
// and the "sheen" source on top with `sheen`, and the tint; with `masked` it is
// cut to the alpha of the "mask" source.
wge::IGraphicsEffect GlassEffect(std::optional<float> blur,
                                 winrt::Windows::UI::Color tint,
                                 bool sheen,
                                 bool masked) {
    auto pv = [](auto v) { return wf::PropertyValue::CreateSingle(v); };

    wge::IGraphicsEffectSource backdrop =
        wuc::CompositionEffectSourceParameter(L"backdrop");
    if (blur) {
        // A hard border extends the edge pixels instead of fading into
        // transparency: each pill piece blurs only its own backdrop, and two
        // soft edges meeting at the joint would show as a seam.
        backdrop = winrt::make<Effect>(
                       L"Blur", CLSID_D2D1GaussianBlur,
                       std::vector<wf::IInspectable>{
                           pv(*blur),
                           wf::PropertyValue::CreateUInt32(kBlurOptimizationBalanced),
                           wf::PropertyValue::CreateUInt32(D2D1_BORDER_MODE_HARD)},
                       std::vector<wge::IGraphicsEffectSource>{backdrop})
                       .as<wge::IGraphicsEffectSource>();
    }
    auto flood = winrt::make<Effect>(
        L"Tint", CLSID_D2D1Flood,
        std::vector<wf::IInspectable>{wf::PropertyValue::CreateSingleArray(
            {tint.R / 255.0f, tint.G / 255.0f, tint.B / 255.0f,
             tint.A / 255.0f})},
        std::vector<wge::IGraphicsEffectSource>{});
    if (sheen) {
        // Glass shows what's behind it a little richer than it is.
        backdrop = winrt::make<Effect>(
                       L"Saturation", CLSID_D2D1Saturation,
                       std::vector<wf::IInspectable>{pv(theme::kGlassSaturation)},
                       std::vector<wge::IGraphicsEffectSource>{backdrop})
                       .as<wge::IGraphicsEffectSource>();
    }
    std::vector<wge::IGraphicsEffectSource> layers{
        backdrop, flood.as<wge::IGraphicsEffectSource>()};
    if (sheen) {
        layers.push_back(wuc::CompositionEffectSourceParameter(L"sheen"));
    }
    auto composite = winrt::make<Effect>(
        L"Composite", CLSID_D2D1Composite,
        std::vector<wf::IInspectable>{
            wf::PropertyValue::CreateUInt32(D2D1_COMPOSITE_MODE_SOURCE_OVER)},
        std::move(layers));
    if (!masked) {
        return composite;
    }
    return winrt::make<Effect>(
        L"Mask", kAlphaMaskEffect, std::vector<wf::IInspectable>{},
        std::vector<wge::IGraphicsEffectSource>{
            composite.as<wge::IGraphicsEffectSource>(),
            wuc::CompositionEffectSourceParameter(L"mask")});
}

class GlassBrush : public Media::XamlCompositionBrushBaseT<GlassBrush> {
   public:
    // With `host` the owner sits in a window of its own (a popup), which has
    // nothing behind it in its tree: the glass shows the desktop through DWM.
    explicit GlassBrush(FrameworkElement const& owner, bool host = false)
        : m_compositor(
              Hosting::ElementCompositionPreview::GetElementVisual(owner)
                  .Compositor()),
          m_owner(owner),
          m_host(host) {
        static int counter;
        m_key = L"PillTaskbarTint" + std::to_wstring(++counter);
        try {
            m_tint = Markup::XamlReader::Load(winrt::hstring(
                         L"<SolidColorBrush xmlns=\"http://schemas.microsoft.com/"
                         L"winfx/2006/xaml/presentation\" Color=\"{ThemeResource " +
                         std::wstring(theme::kTintResource) + L"}\"/>"))
                         .as<Media::SolidColorBrush>();
            owner.Resources().Insert(box_value(m_key), m_tint);
            m_tint.RegisterPropertyChangedCallback(
                Media::SolidColorBrush::ColorProperty(),
                [weak = get_weak()](auto&&, auto&&) {
                    if (auto self = weak.get()) {
                        core::Guard(L"Tint refresh", [&] { self->Refresh(); });
                    }
                });
        } catch (...) {
            Wh_Log(L"Tint brush failed: %08X", winrt::to_hresult());
            m_tint = nullptr;
        }
        if (settings::g.pill.style == settings::Style::kBloom && !host) {
            // Kept in the owner's resources, like the tint, to follow its theme.
            try {
                m_edge = Markup::XamlReader::Load(winrt::hstring(
                             L"<SolidColorBrush xmlns=\"http://schemas.microsoft.com/"
                             L"winfx/2006/xaml/presentation\" Color=\"{ThemeResource " +
                             std::wstring(theme::kEdgeResource) + L"}\" Opacity=\"" +
                             std::to_wstring(theme::kEdgeOpacity) + L"\"/>"))
                             .as<Media::SolidColorBrush>();
                owner.Resources().Insert(box_value(m_key + L"Edge"), m_edge);
            } catch (...) {
                Wh_Log(L"Edge brush failed: %08X", winrt::to_hresult());
                m_edge = nullptr;
            }
        }
    }

    ~GlassBrush() {
        auto owner = m_owner.get();
        for (auto [brush, key] : {std::pair{m_tint, m_key}, std::pair{m_edge, m_key + L"Edge"}}) {
            if (owner && brush) {
                try {
                    owner.Resources().Remove(box_value(key));
                } catch (...) {
                }
            }
        }
    }

    // The Bloom style's outline, null in the others.
    Media::Brush Edge() const {
        return m_edge;
    }

    void OnConnected() {
        if (CompositionBrush()) {
            return;
        }
        auto tint = m_tint ? m_tint.Color() : winrt::Windows::UI::Color{};
        // Without glass the tint is the whole fill, opaque.
        bool glass = settings::g.pill.glass;
        tint.A = glass ? (uint8_t)std::lround(settings::g.pill.tintOpacity * 2.55) : 255;
        if (glass && settings::g.pill.style == settings::Style::kGlass) {
            core::Guard(L"Glass brush",
                        [&] { CompositionBrush(CreateBlur(tint, true)); });
        }
        if (glass && !CompositionBrush()) {
            core::Guard(L"Blur brush",
                        [&] { CompositionBrush(CreateBlur(tint, false)); });
        }
        if (!CompositionBrush()) {
            core::Guard(L"Tint brush",
                        [&] { CompositionBrush(m_compositor.CreateColorBrush(tint)); });
        }
    }

    void OnDisconnected() {
        core::Guard(L"Brush release", [&] {
            if (auto brush = CompositionBrush()) {
                brush.Close();
                CompositionBrush(nullptr);
            }
        });
    }

   private:
    wuc::CompositionBrush CreateBlur(winrt::Windows::UI::Color tint, bool sheen) {
        auto brush =
            m_compositor
                .CreateEffectFactory(
                    GlassEffect(settings::g.pill.blur, tint, sheen, false))
                .CreateBrush();
        brush.SetSourceParameter(L"backdrop",
                                 m_host ? m_compositor.CreateHostBackdropBrush()
                                        : m_compositor.CreateBackdropBrush());
        if (sheen) {
            brush.SetSourceParameter(L"sheen", Sheen(m_compositor));
        }
        return brush;
    }

    void Refresh() {
        if (CompositionBrush()) {
            OnDisconnected();
            OnConnected();
        }
    }

    wuc::Compositor m_compositor;
    winrt::weak_ref<FrameworkElement> m_owner;
    bool m_host;
    Media::SolidColorBrush m_tint{nullptr};
    Media::SolidColorBrush m_edge{nullptr};
    std::wstring m_key;
};

// ---- pieces -------------------------------------------------------------------

CornerRadius Corners(double l, double r) {
    return {l, r, r, l};
}

// The glass and Bloom styles' edge is one physical pixel: a hairline disappears at 100%
// scale, and a whole DIP at a fractional scale spills into a second,
// half-covered pixel row that reads as a rough edge.
double PhysicalPixel(FrameworkElement const& element) {
    if (auto root = element.XamlRoot(); root && root.RasterizationScale() > 0) {
        return 1.0 / root.RasterizationScale();
    }
    return 1.0;
}

Media::LinearGradientBrush Rim() {
    Media::LinearGradientBrush gradient;
    gradient.StartPoint({0, 0});
    gradient.EndPoint({0, 1});
    for (auto [offset, color] : theme::kGlassRim) {
        Media::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        gradient.GradientStops().Append(stop);
    }
    return gradient;
}

// Margin, padding, border, corners and background of a pill piece: a Grid
// (RootGrid, older tray) or a StackPanel (newer tray).
void StylePiece(FrameworkElement const& piece,
                Thickness margin,
                Thickness padding,
                Thickness border,
                CornerRadius corners) {
    core::Set(piece, FrameworkElement::MarginProperty(), box_value(margin),
              L"Margin");
    auto glass = winrt::make_self<GlassBrush>(piece);
    auto brush = glass.as<Media::Brush>();
    Media::Brush edge{nullptr};
    if (settings::g.pill.style != settings::Style::kWindowGlass) {
        double pixel = PhysicalPixel(piece);
        auto full = [pixel](double v) { return v > 0 ? pixel : 0.0; };
        border = {full(border.Left), full(border.Top), full(border.Right),
                  full(border.Bottom)};
        edge = settings::g.pill.style == settings::Style::kGlass ? Rim() : glass->Edge();
    }
    if (piece.try_as<Controls::Grid>()) {
        using G = Controls::Grid;
        core::Set(piece, G::PaddingProperty(), box_value(padding), L"Padding");
        core::Set(piece, G::BorderThicknessProperty(), box_value(border),
                  L"BorderThickness");
        core::Set(piece, G::CornerRadiusProperty(), box_value(corners),
                  L"CornerRadius");
        core::Set(piece, Controls::Panel::BackgroundProperty(), brush,
                  L"Background");
        if (edge) {
            core::Set(piece, G::BorderBrushProperty(), edge, L"BorderBrush");
        }
    } else if (piece.try_as<Controls::StackPanel>()) {
        using S = Controls::StackPanel;
        core::Set(piece, S::PaddingProperty(), box_value(padding), L"Padding");
        core::Set(piece, S::BorderThicknessProperty(), box_value(border),
                  L"BorderThickness");
        core::Set(piece, S::CornerRadiusProperty(), box_value(corners),
                  L"CornerRadius");
        core::Set(piece, Controls::Panel::BackgroundProperty(), brush,
                  L"Background");
        if (edge) {
            core::Set(piece, S::BorderBrushProperty(), edge, L"BorderBrush");
        }
    } else {
        Wh_Log(L"Unsupported piece %s", winrt::get_class_name(piece).c_str());
    }
}

FrameworkElement TrayFrameNextTo(FrameworkElement const& frame) {
    return core::ChildOfClass(core::Parent(frame), L"SystemTray.SystemTrayFrame");
}

void StyleAppsPiece(FrameworkElement const& frame, bool withTray) {
    auto rootGrid = core::ChildNamed(frame, L"RootGrid");
    if (!rootGrid) {
        Wh_Log(L"No RootGrid");
        return;
    }
    double r = settings::g.pill.cornerRadius;
    if (withTray) {
        StylePiece(rootGrid, theme::kAppsMargin, theme::kAppsPadding,
                   theme::kAppsBorder, Corners(r, 0));
    } else {
        StylePiece(rootGrid, theme::kSoloMargin, theme::kSoloPadding,
                   theme::kSoloBorder, Corners(r, r));
    }
}

void StyleTray(FrameworkElement const& trayFrame) {
    core::Set(trayFrame, Controls::Grid::ColumnProperty(), box_value(2));
    core::Set(trayFrame, FrameworkElement::WidthProperty(),
              box_value(std::numeric_limits<double>::quiet_NaN()));
    core::Set(trayFrame, FrameworkElement::HorizontalAlignmentProperty(),
              box_value(HorizontalAlignment::Left));
    core::Set(trayFrame, FrameworkElement::MarginProperty(),
              box_value(Thickness{}));
    if (auto grid = core::ChildNamed(trayFrame, L"SystemTrayFrameGrid")) {
        double r = settings::g.pill.cornerRadius;
        StylePiece(grid, settings::TrayMargin(), theme::kTrayPadding,
                   theme::kTrayBorder, Corners(0, r));
    } else {
        Wh_Log(L"No SystemTrayFrameGrid");
    }
}

void StyleBackground(FrameworkElement const& background) {
    // BackgroundControl > Grid > Rectangle#BackgroundFill, #BackgroundStroke.
    // Touching Fill before the template is applied crashes explorer; hiding
    // the rectangles doesn't need it.
    auto grid = core::ChildOfClass(background, L"Windows.UI.Xaml.Controls.Grid");
    for (PCWSTR name : {L"BackgroundFill", L"BackgroundStroke"}) {
        if (auto rect = core::ChildNamed(grid, name)) {
            core::Set(rect, UIElement::VisibilityProperty(),
                      box_value(Visibility::Collapsed));
        }
    }
}

// The taskbar lights the focused app's button (BackgroundElement's fill and
// stroke) through its Active* visual states. After each state pass a button in
// one of them goes to its inactive twin, which keeps the pointer feedback. The
// running line has its own group (RunningIndicatorStates) and stays.
constexpr std::pair<std::wstring_view, std::wstring_view> kUnlitStates[] = {
    {L"ActiveNormal", L"InactiveNormal"},
    {L"ActivePointerOver", L"InactivePointerOver"},
    {L"ActivePressed", L"InactivePressed"},
    {L"MultiWindowActive", L"MultiWindowNormal"},
    {L"WindowSharingActive", L"WindowSharingNormal"},
    {L"WindowSharingActivePointerOver", L"WindowSharingPointerOver"},
    {L"WindowSharingActivePressed", L"WindowSharingPressed"},
    {L"MultiWindowSharingActive", L"MultiWindowSharingNormal"},
};

// A state the mod replaced, for Restore to put back.
struct Unlit {
    winrt::weak_ref<Controls::Control> button;
    winrt::weak_ref<VisualStateGroup> group;
    std::wstring_view active, inactive;
};
std::vector<Unlit> g_unlit;

void Unlight(Controls::Control const& button) {
    std::erase_if(g_unlit, [&](auto& u) {
        auto b = u.button.get();
        return !b || b == button;
    });
    if (Media::VisualTreeHelper::GetChildrenCount(button) == 0) {
        return;  // no template yet
    }
    auto root = Media::VisualTreeHelper::GetChild(button, 0).try_as<FrameworkElement>();
    if (!root) {
        return;
    }
    for (auto const& group : VisualStateManager::GetVisualStateGroups(root)) {
        auto state = group.CurrentState();
        if (!state) {
            continue;
        }
        auto name = state.Name();
        for (auto& [active, inactive] : kUnlitStates) {
            if (name == active) {
                if (VisualStateManager::GoToState(button, inactive, true)) {
                    g_unlit.push_back({winrt::make_weak(button), winrt::make_weak(group),
                                       active, inactive});
                }
                break;
            }
        }
    }
}

// Buttons still in a state the mod chose go back to the taskbar's own.
void RelightAll() {
    for (auto& u : g_unlit) {
        core::Guard(L"Relight", [&] {
            auto button = u.button.get();
            auto group = u.group.get();
            auto state = group ? group.CurrentState() : nullptr;
            if (button && state && state.Name() == u.inactive) {
                VisualStateManager::GoToState(button, u.active, false);
            }
        });
    }
    g_unlit.clear();
}

void StyleTaskButton(FrameworkElement const& button) {
    if (core::g_vertical) {
        return;
    }
    auto control = button.try_as<Controls::Control>();
    if (control) {
        Unlight(control);
    }
    CornerRadius r{theme::kTaskButtonRadius, theme::kTaskButtonRadius,
                   theme::kTaskButtonRadius, theme::kTaskButtonRadius};
    if (control && control.CornerRadius() != r) {
        core::Set(control, Controls::Control::CornerRadiusProperty(),
                  box_value(r));
    }
}

void StyleTrayButton(FrameworkElement const& e) {
    auto control = e.try_as<Controls::Control>();
    if (!control) {
        return;
    }
    using C = Controls::Control;
    double r = theme::kTrayButtonRadius;
    core::Set(control, C::PaddingProperty(),
              box_value(theme::kTrayButtonPadding));
    core::Set(control, C::CornerRadiusProperty(),
              box_value(CornerRadius{r, r, r, r}));
    core::Set(control, FrameworkElement::MarginProperty(),
              box_value(theme::kTrayButtonMargin));
}

void StyleClock(FrameworkElement const& icon) {
    auto find = [&](PCWSTR name) {
        return core::FindDescendant(
                   icon, [&](auto& e) { return e.Name() == name; }, 8)
            .try_as<Controls::TextBlock>();
    };
    if (auto time = find(L"TimeInnerTextBlock")) {
        using T = Controls::TextBlock;
        core::Keep(time, T::FontSizeProperty(), box_value(theme::kClockFontSize));
        core::Set(time, T::FontFamilyProperty(),
                  Media::FontFamily(theme::kClockFont));
        core::Set(time, FrameworkElement::MarginProperty(),
                  box_value(Thickness{}));
        core::Set(time, T::PaddingProperty(), box_value(Thickness{}));
        core::Set(time, UIElement::RenderTransformProperty(),
                  Media::TranslateTransform());
    }
}

// Windows' "select the far corner of the taskbar to show the desktop", on
// unless turned off.
bool ShowDesktopCorner() {
    DWORD on = 1, size = sizeof(on);
    RegGetValue(HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                L"TaskbarSd", RRF_RT_REG_DWORD, nullptr, &on, &size);
    return on != 0;
}

// A tray icon view (SystemTray.IconView and the classes built on it).
void StyleIcon(FrameworkElement const& icon) {
    if (core::g_vertical) {
        return;
    }
    auto cls = winrt::get_class_name(icon);
    if (cls == L"SystemTray.ChevronIconView" ||
        (cls == L"SystemTray.NotifyIconView" && icon.Name() == L"NotifyItemIcon")) {
        StyleTrayButton(icon);
        return;
    }
    if (cls == L"SystemTray.CopilotIcon") {
        auto control = icon.try_as<Controls::Control>();
        if (control) {
            core::Set(control, Controls::Control::PaddingProperty(),
                      box_value(theme::kTrayButtonPadding));
            core::Set(control, FrameworkElement::MarginProperty(),
                      box_value(theme::kTrayButtonMargin));
        }
        return;
    }
    // The clock's text blocks sit in different icon views across builds.
    StyleClock(icon);
    if (icon.Name() != L"SystemTrayIcon") {
        return;
    }

    if (auto omni = core::FindAncestor(
            icon, [](auto& p) { return core::IsClass(p, L"SystemTray.OmniButton"); })) {
        StyleTrayButton(omni);
    }

    auto container = core::ChildNamed(icon, L"ContainerGrid");
    if (core::AncestorNamed(icon, L"NotificationCenterButton")) {
        if (auto grid =
                core::ChildOfClass(icon, L"Windows.UI.Xaml.Controls.Grid")) {
            core::Set(grid, Controls::Grid::PaddingProperty(),
                      box_value(theme::kBellPadding));
        }
    }

    // ContainerGrid > ContentPresenter > ContentGrid > TextIconContent >
    // ContainerGrid: the rounded highlight of the glyph icons.
    auto text = core::ChildOfClass(
        core::ChildNamed(core::ChildNamed(container, L"ContentPresenter"),
                         L"ContentGrid"),
        L"SystemTray.TextIconContent");
    if (auto grid = core::ChildNamed(text, L"ContainerGrid").try_as<Controls::Grid>()) {
        double p = theme::kTextIconPadding, r = theme::kTextIconRadius;
        core::Set(grid, Controls::Grid::PaddingProperty(),
                  box_value(Thickness{p, p, p, p}));
        core::Set(grid, Controls::Grid::CornerRadiusProperty(),
                  box_value(CornerRadius{r, r, r, r}));
    }

    if (core::AncestorNamed(icon, L"IconStack")) {
        if (auto control = icon.try_as<Controls::Control>()) {
            core::Set(control, Controls::Control::PaddingProperty(),
                      box_value(Thickness{}));
        }
    }
    // The show-desktop sliver follows its Windows setting; when it's off, the
    // sliver would only split the pill.
    if (auto stack = core::AncestorNamed(icon, L"ShowDesktopStack")) {
        core::Set(stack, UIElement::VisibilityProperty(),
                  box_value(ShowDesktopCorner() ? Visibility::Visible
                                                : Visibility::Collapsed));
    }
}

// ---- taskbar ------------------------------------------------------------------

struct Container {
    winrt::weak_ref<FrameworkElement> grid;
    FrameworkElement::SizeChanged_revoker sizeChanged;
};

std::list<Container> g_containers;
std::list<FrameworkElement::Loaded_revoker> g_pendingLoads;

double FrameMaxWidth(FrameworkElement const& container) {
    double width = container.ActualWidth();
    return width > 0 ? std::min(theme::kFrameMaxWidth, width)
                     : theme::kFrameMaxWidth;
}

// The taskbar frame and the tray sit side by side in the two Auto columns of
// the full-width content grid. The free columns around them put the pill in the
// middle, or one of them closes to set it against that side.
void StyleFrame(FrameworkElement const& frame, HWND hwnd) {
    auto container = core::Parent(frame);
    auto grid = container.try_as<Controls::Grid>();
    if (!grid) {
        Wh_Log(L"Unexpected taskbar frame parent");
        return;
    }

    auto tray = TrayFrameNextTo(frame);
    Wh_Log(L"Container %s, %d columns, tray %s",
           winrt::get_class_name(container).c_str(),
           (int)grid.ColumnDefinitions().Size(), tray ? L"beside" : L"none");

    auto align = settings::PillAlign();
    auto side = [](bool open) {
        return open ? GridLength{1, GridUnitType::Star} : GridLength{0, GridUnitType::Pixel};
    };
    core::SetColumns(grid, {side(align != settings::Align::kLeft),
                            GridLength{1, GridUnitType::Auto},
                            GridLength{1, GridUnitType::Auto},
                            side(align != settings::Align::kRight)});
    core::Set(frame, Controls::Grid::ColumnProperty(), box_value(1));
    // Explorer gives a secondary taskbar's frame the width the tray leaves
    // again later (as the clock's width changes), which stretches the pill.
    core::Keep(frame, FrameworkElement::WidthProperty(),
               box_value(std::numeric_limits<double>::quiet_NaN()));
    core::Set(frame, FrameworkElement::MinWidthProperty(),
              box_value(theme::kFrameMinWidth));
    core::Set(frame, FrameworkElement::MaxWidthProperty(),
              box_value(FrameMaxWidth(container)));

    bool known = std::any_of(g_containers.begin(), g_containers.end(),
                             [&](auto& c) { return c.grid.get() == container; });
    if (!known) {
        auto& c = g_containers.emplace_back();
        c.grid = container;
        c.sizeChanged = container.SizeChanged(
            winrt::auto_revoke,
            [weakFrame = winrt::make_weak(frame)](auto&& sender, auto&&) {
                auto frame = weakFrame.get();
                auto container = sender.template try_as<FrameworkElement>();
                if (frame && container) {
                    core::Guard(L"Frame width", [&] {
                        core::Set(frame, FrameworkElement::MaxWidthProperty(),
                                  box_value(FrameMaxWidth(container)));
                    });
                }
            });
    }

    // The tray goes to its column first: a failed piece must not leave it in
    // the stock column, left of the apps.
    if (tray) {
        core::Guard(L"Tray", [&] { StyleTray(tray); });
    }
    core::Guard(L"Apps piece", [&] { StyleAppsPiece(frame, (bool)tray); });

    if (auto gripper = core::FindDescendant(
            frame, [](auto& e) { return e.Name() == L"GripperControl"; }, 12)) {
        core::Set(gripper, FrameworkElement::WidthProperty(),
                  box_value(std::numeric_limits<double>::quiet_NaN()));
        core::Set(gripper, FrameworkElement::MinWidthProperty(),
                  box_value(theme::kGripperMinWidth));
    }
    core::FindDescendant(
        frame,
        [](auto& e) {
            if (e.Name() == L"ExperienceToggleButtonRootPanel") {
                core::Set(e, UIElement::RenderTransformProperty(),
                          Media::TranslateTransform());
            }
            return false;
        },
        12);

    region::Track(frame, hwnd);
}

// Everything already in a taskbar's tree.
void StyleRoot(XamlRoot const& root, HWND hwnd) {
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    if (!content) {
        return;
    }
    // A theme set (Bloom's own) over Windows': all under the root follows it.
    if (auto set = settings::g.pill.theme; set != settings::Theme::kSystem) {
        core::Set(content, FrameworkElement::RequestedThemeProperty(),
                  box_value(set == settings::Theme::kDark ? ElementTheme::Dark
                                                          : ElementTheme::Light));
    }
    if (auto frame = core::FindDescendant(
            content, [](auto& e) { return core::IsClass(e, L"Taskbar.TaskbarFrame"); },
            8)) {
        core::Guard(L"Frame", [&] { StyleFrame(frame, hwnd); });
    } else {
        Wh_Log(L"No TaskbarFrame");
    }
    if (auto bg = core::FindDescendant(
            content, [](auto& e) { return e.Name() == L"BackgroundControl"; }, 8)) {
        core::Guard(L"Background", [&] { StyleBackground(bg); });
    }
    core::FindDescendant(
        content,
        [](auto& e) {
            auto cls = winrt::get_class_name(e);
            if (cls == L"Taskbar.TaskListButton") {
                core::Guard(L"Task button", [&] { StyleTaskButton(e); });
            } else if (cls.starts_with(L"SystemTray.") &&
                       (cls.ends_with(L"IconView") || cls == L"SystemTray.CopilotIcon")) {
                core::Guard(L"Tray icon", [&] { StyleIcon(e); });
            }
            return false;
        },
        40);
}

// A taskbar frame whose template was just applied, e.g. a new monitor's.
void OnFrameTemplate(FrameworkElement const& frame) {
    if (core::g_vertical) {
        return;
    }
    auto it = g_pendingLoads.emplace(g_pendingLoads.end());
    *it = frame.Loaded(winrt::auto_revoke, [it](auto&& sender, auto&&) {
        auto frame = sender.template try_as<FrameworkElement>();
        g_pendingLoads.erase(it);  // destroys this handler; locals only below
        if (frame && !core::g_unloading) {
            core::Guard(L"New taskbar", [&] { StyleRoot(frame.XamlRoot(), nullptr); });
        }
    });
}

// A tray icon view loaded. The tray of a new taskbar shows up this way, once
// its first icon loads, so the tray piece is styled here too.
void OnIconLoaded(FrameworkElement const& icon) {
    if (core::g_vertical) {
        return;
    }
    StyleIcon(icon);
    auto trayFrame = core::FindAncestor(icon, [](auto& p) {
        return core::IsClass(p, L"SystemTray.SystemTrayFrame");
    });
    auto grid = core::ChildNamed(trayFrame, L"SystemTrayFrameGrid");
    if (!grid || core::SameValue(grid.ReadLocalValue(FrameworkElement::MarginProperty()),
                                 box_value(settings::TrayMargin()))) {
        return;  // styled already
    }
    StyleTray(trayFrame);
    if (auto frame = core::ChildOfClass(core::Parent(trayFrame),
                                        L"Taskbar.TaskbarFrame")) {
        StyleAppsPiece(frame, true);
    }
}

void Apply(HWND hwnd) {
    if (!core::g_vertical) {
        StyleRoot(core::TaskbarRoot(hwnd), hwnd);
    }
}

void Restore() {
    g_pendingLoads.clear();
    g_containers.clear();
    RelightAll();
    region::Clear();
}

}  // namespace pill

// ============================================================================
namespace widgets {
// ============================================================================
// The media player and the usage meter sit in horizontal panels inserted in
// each taskbar's tray (StackPanel#SystemTrayFrameGrid), one before the tray
// icons and one after the clock, so they are part of the tray piece of the
// pill. Every taskbar gets elements of its own: one can't be in two trees.
// Taskbar thread only.

enum class Side { kStart, kEnd };

constexpr PCWSTR kHostNames[] = {L"PillWidgets", L"PillWidgetsEnd"};
constexpr double kHostGap = 6;  // between a panel and the tray icons beside it

// An element stays a child of a destroyed tray's panel; it can't join another.
void Detach(FrameworkElement const& element) {
    if (auto panel = core::Parent(element).try_as<Controls::Panel>()) {
        uint32_t index;
        if (panel.Children().IndexOf(element, index)) {
            panel.Children().RemoveAt(index);
        }
    }
}

struct Item {
    std::wstring name;
    int order;
    Side side;
    FrameworkElement element{nullptr};
};

// One taskbar's widgets and the panels they sit in.
struct Bar {
    HWND hwnd = nullptr;
    std::vector<Item> items;  // sorted by order
    winrt::weak_ref<Controls::StackPanel> hosts[2];
    bool noTray = false;  // logged that it has no tray to hold them
};

std::list<Bar> g_bars;

Bar& BarOf(HWND hwnd) {
    std::erase_if(g_bars, [](Bar const& b) { return !IsWindow(b.hwnd); });
    for (auto& bar : g_bars) {
        if (bar.hwnd == hwnd) {
            return bar;
        }
    }
    auto& bar = g_bars.emplace_back();
    bar.hwnd = hwnd;
    return bar;
}

// Just after the child holding the clock and the show-desktop sliver that
// follows it, so the pill ends with the widgets; else at the end.
uint32_t EndIndex(Controls::StackPanel const& tray) {
    auto children = tray.Children();
    auto named = [&](uint32_t i, PCWSTR name) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        return child && child.Name() == name;
    };
    for (uint32_t i = 0; i < children.Size(); i++) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        if (child && (child.Name() == L"NotificationCenterButton" ||
                      core::FindDescendant(
                          child,
                          [](auto& e) { return e.Name() == L"NotificationCenterButton"; },
                          3))) {
            return i + 1 < children.Size() && named(i + 1, L"ShowDesktopStack") ? i + 2
                                                                                : i + 1;
        }
    }
    return children.Size();
}

Controls::StackPanel Host(Bar& bar, Side side) {
    auto& cached = bar.hosts[(int)side];
    if (auto host = cached.get(); host && core::Parent(host)) {
        return host;
    }
    auto root = core::TaskbarRoot(bar.hwnd);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    auto trayFrame = core::FindDescendant(
        content, [](auto& e) { return core::IsClass(e, L"SystemTray.SystemTrayFrame"); },
        8);
    auto tray = core::ChildNamed(trayFrame, L"SystemTrayFrameGrid")
                    .try_as<Controls::StackPanel>();
    if (!tray) {
        // shortcut: older builds have a Grid here; add a column there to
        // support them.
        if (!std::exchange(bar.noTray, true)) {
            Wh_Log(L"No StackPanel tray for widgets on %08X",
                   (DWORD)(ULONG_PTR)bar.hwnd);
        }
        return nullptr;
    }
    PCWSTR name = kHostNames[(int)side];
    if (auto existing = core::ChildNamed(tray, name).try_as<Controls::StackPanel>()) {
        cached = existing;
        return existing;
    }
    Controls::StackPanel host;
    host.Name(name);
    host.Orientation(Controls::Orientation::Horizontal);
    host.VerticalAlignment(VerticalAlignment::Center);
    host.Spacing(8);
    if (side == Side::kStart) {
        host.Margin({0, 0, kHostGap, 0});
        tray.Children().InsertAt(0, host);
    } else {
        host.Margin({kHostGap, 0, 0, 0});
        tray.Children().InsertAt(EndIndex(tray), host);
    }
    cached = host;
    for (auto& item : bar.items) {
        if (item.side == side) {
            Detach(item.element);
            host.Children().Append(item.element);
        }
    }
    return host;
}

void RemoveFromHost(Bar& bar, Item const& item) {
    if (auto host = bar.hosts[(int)item.side].get()) {
        uint32_t index;
        if (host.Children().IndexOf(item.element, index)) {
            host.Children().RemoveAt(index);
        }
    }
}

// Puts `element` in the widget panel of `side` of the taskbar under `name`,
// replacing the previous one of that name there. Cheap when it's already
// there, so updates may call it each time.
void Add(HWND taskbar,
         PCWSTR name,
         int order,
         FrameworkElement const& element,
         Side side = Side::kStart) {
    if (core::g_vertical || core::g_unloading || !taskbar) {
        return;
    }
    auto& bar = BarOf(taskbar);
    auto& items = bar.items;
    auto it = std::find_if(items.begin(), items.end(),
                           [&](auto& i) { return i.name == name; });
    auto host = bar.hosts[(int)side].get();
    if (it != items.end() && it->element == element && it->side == side && host &&
        core::Parent(host) && core::Parent(element) == host) {
        return;
    }
    if (it != items.end()) {
        RemoveFromHost(bar, *it);
        items.erase(it);
    }
    auto pos = std::find_if(items.begin(), items.end(),
                            [&](auto& i) { return i.order > order; });
    size_t index = std::count_if(items.begin(), pos,
                                 [&](auto& i) { return i.side == side; });
    items.insert(pos, Item{name, order, side, element});
    host = Host(bar, side);
    if (!host) {
        return;
    }
    Detach(element);
    host.Children().InsertAt((uint32_t)std::min<size_t>(index, host.Children().Size()),
                             element);
}

// As tray icons load: widgets that came before their tray did (a young
// explorer) go into it now.
void PlaceWaiting() {
    if (core::g_vertical || core::g_unloading) {
        return;
    }
    for (auto& bar : g_bars) {
        for (Side side : {Side::kStart, Side::kEnd}) {
            auto host = bar.hosts[(int)side].get();
            if ((!host || !core::Parent(host)) && IsWindow(bar.hwnd) &&
                std::ranges::any_of(bar.items, [&](auto& i) { return i.side == side; })) {
                Host(bar, side);
            }
        }
    }
}

void Remove(HWND taskbar, PCWSTR name) {
    for (auto& bar : g_bars) {
        if (bar.hwnd != taskbar) {
            continue;
        }
        auto it = std::find_if(bar.items.begin(), bar.items.end(),
                               [&](auto& i) { return i.name == name; });
        if (it != bar.items.end()) {
            RemoveFromHost(bar, *it);
            bar.items.erase(it);
        }
    }
}

void Restore() {
    for (auto& bar : g_bars) {
        for (auto& cached : bar.hosts) {
            if (auto host = cached.get()) {
                if (auto tray = core::Parent(host).try_as<Controls::Panel>()) {
                    uint32_t index;
                    if (tray.Children().IndexOf(host, index)) {
                        tray.Children().RemoveAt(index);
                    }
                }
            }
        }
    }
    g_bars.clear();
}

}  // namespace widgets

// ============================================================================
namespace tray {
// ============================================================================
// Hides tray system icons, the bell and the clock's date. A system icon's kind
// comes from the glyph it shows; icons are handled as their views load.

using winrt::box_value;
using settings::Bell;
using settings::Clock;

enum class Icon {
    kUnknown,
    kNone,
    kVolume,
    kNetwork,
    kBattery,
    kMicrophone,
    kGeolocation,
    kMicrophoneAndGeolocation,
    kBellEmpty,
    kBellEmptyDnd,
    kBellFull,
    kBellFullDnd,
    kLanguage,
    kStudioEffects,
    kRecall,
};

Icon Identify(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return Icon::kNone;

        case 1:
            break;

        default:
            return Icon::kUnknown;
    }

    switch (text[0]) {
        case L'\uE74F':  // Mute
        case L'\uE992':  // Volume0
        case L'\uE993':  // Volume1
        case L'\uE994':  // Volume2
        case L'\uE995':  // Volume3
        case L'\uEA85':  // VolumeDisabled
        case L'\uEBC5':  // VolumeBars
            return Icon::kVolume;

        case L'\uE709':  // Airplane
        case L'\uE7F4':  // TVMonitor
        case L'\uE839':  // Ethernet
        case L'\uE86C':  // SignalBars1
        case L'\uE86D':  // SignalBars2
        case L'\uE86E':  // SignalBars3
        case L'\uE86F':  // SignalBars4
        case L'\uE870':  // SignalBars5
        case L'\uEAA1':  // Drinks
        case L'\uEAA2':  // DropShot
        case L'\uEAA3':  // FlavorProfile
        case L'\uEAA4':  // Float
        case L'\uEAA5':  // FluteGlass
        case L'\uEAA8':  // GobletGlass
        case L'\uEC1E':  // SignalRoaming
        case L'\uEC3C':  // MobWifi1
        case L'\uEC3D':  // MobWifi2
        case L'\uEC3E':  // MobWifi3
        case L'\uEC3F':  // MobWifi4
        case L'\uF384':  // NetworkOffline
        case L'\uF8C0':  // SysLocationArrow
        case L'\uF8C1':  // SysMicrophone
        case L'\uF8C2':  // SysVideo
        case L'\uF8C3':  // SysWifi4
        case L'\uF8C4':  // SysWifi3
        case L'\uF8C5':  // SysWifi2
        case L'\uF8C6':  // SysWifi1
        case L'\uF8C7':  // SysSignalBars5
        case L'\uF8C8':  // SysSignalBars4
        case L'\uF8C9':  // SysSignalBars3
        case L'\uF8CA':  // SysSignalBars2
        case L'\uF8CB':  // SysSignalBars1
        case L'\uF8CC':  // SysNetworkOffline
            return Icon::kNetwork;

        // Charging levels.
        case L'\uE3C1':  // Private Use
        case L'\uE3C2':  // Private Use
        case L'\uE3C3':  // Private Use
        case L'\uE3C4':  // Private Use
        case L'\uE3C5':  // Private Use
        case L'\uE3C6':  // Private Use
        case L'\uE3C7':  // Private Use
        case L'\uE3C8':  // Private Use
        case L'\uE3C9':  // Private Use
        case L'\uE3CA':  // Private Use
        case L'\uE3CB':  // Private Use
        case L'\uE408':  // Private Use
        case L'\uE409':  // Private Use
        case L'\uE40A':  // Private Use
        case L'\uE40B':  // Private Use
        case L'\uE40C':  // Private Use
        case L'\uE40D':  // Private Use
        case L'\uE40E':  // Private Use
        case L'\uE40F':  // Private Use
        case L'\uE410':  // Private Use
        case L'\uE411':  // Private Use
        case L'\uE412':  // Private Use
        case L'\uE413':  // Private Use
        case L'\uE414':  // Private Use
        case L'\uE415':  // Private Use
        case L'\uE416':  // Private Use
        case L'\uE417':  // Private Use
        case L'\uE418':  // Private Use
        case L'\uE419':  // Private Use
        case L'\uE41A':  // Private Use
        case L'\uE41B':  // Private Use
        case L'\uE41C':  // Private Use
        case L'\uE41D':  // Private Use
        case L'\uEBA0':  // MobBattery0
        case L'\uEBA1':  // MobBattery1
        case L'\uEBA2':  // MobBattery2
        case L'\uEBA3':  // MobBattery3
        case L'\uEBA4':  // MobBattery4
        case L'\uEBA5':  // MobBattery5
        case L'\uEBA6':  // MobBattery6
        case L'\uEBA7':  // MobBattery7
        case L'\uEBA8':  // MobBattery8
        case L'\uEBA9':  // MobBattery9
        case L'\uEBAA':  // MobBattery10
        case L'\uEBAB':  // MobBatteryCharging0
        case L'\uEBAC':  // MobBatteryCharging1
        case L'\uEBAD':  // MobBatteryCharging2
        case L'\uEBAE':  // MobBatteryCharging3
        case L'\uEBAF':  // MobBatteryCharging4
        case L'\uEBB0':  // MobBatteryCharging5
        case L'\uEBB1':  // MobBatteryCharging6
        case L'\uEBB2':  // MobBatteryCharging7
        case L'\uEBB3':  // MobBatteryCharging8
        case L'\uEBB4':  // MobBatteryCharging9
        case L'\uEBB5':  // MobBatteryCharging10
        case L'\uEBB6':  // MobBatterySaver0
        case L'\uEBB7':  // MobBatterySaver1
        case L'\uEBB8':  // MobBatterySaver2
        case L'\uEBB9':  // MobBatterySaver3
        case L'\uEBBA':  // MobBatterySaver4
        case L'\uEBBB':  // MobBatterySaver5
        case L'\uEBBC':  // MobBatterySaver6
        case L'\uEBBD':  // MobBatterySaver7
        case L'\uEBBE':  // MobBatterySaver8
        case L'\uEBBF':  // MobBatterySaver9
        case L'\uEBC0':  // MobBatterySaver10
        // Misc.
        case L'\uEB17':
        case L'\uEC02':
        case L'\uF1E8':
            return Icon::kBattery;

        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return Icon::kMicrophone;

        case L'\uE37A':
            return Icon::kGeolocation;

        case L'\uF47F':
            return Icon::kMicrophoneAndGeolocation;

        case L'\uF2A3':  // Empty bell
            return Icon::kBellEmpty;

        case L'\uF285':  // Empty bell, Do Not Disturb
            return Icon::kBellEmptyDnd;

        case L'\uF2A5':  // Full bell
            return Icon::kBellFull;

        case L'\uF2A8':  // Full bell, Do Not Disturb
            return Icon::kBellFullDnd;

        // Language supplementary icons.
        // Found by installing all the built-in input methods from:
        // https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11#input-method-editors
        // and identifying the icon code in the fonts Segoe Fluent and
        // AXPIcons.ttf.
        // https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font
        // %SystemRoot%\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\AXPIcons.ttf
        case L'\uE4D7':  // (Maybe) English Private mode
        case L'\uE4D8':  // (Maybe) Chinese Private mode
        case L'\uE5BF':  // (Maybe) English mode locked
        case L'\uE97E':  // HalfAlpha
        case L'\uE97F':  // FullAlpha
        case L'\uE980':  // Key12On (Korean mode)
        case L'\uE982':  // QWERTYOn (Chinese mode)
        case L'\uE983':  // QWERTYOff (English mode)
        case L'\uE986':  // FullHiragana
        case L'\uE987':  // FullKatakana
        case L'\uE988':  // HalfKatakana
        case L'\uEB90':  // StatusErrorFull (Input method disabled)
        case L'\uEE41':  // FullHiraganaPrivateMode
        case L'\uEE42':  // FullKatakanaPrivateMode
        case L'\uEE43':  // HalfAlphaPrivateMode
        case L'\uEE44':  // HalfKatakanaPrivateMode
        case L'\uEE45':  // FullAlphaPrivateMode
        case L'\uEE75':  // (Maybe) HalfAlpha
        case L'\uEE76':  // (Maybe) HalfAlphaPrivateMode
            return Icon::kLanguage;

        case L'\uEABC':
            return Icon::kStudioEffects;

        // From:
        // C:\Windows\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\RecallIcons.ttf
        case L'\uEC83':
        case L'\uEADD':
        case L'\uEB16':
        case L'\uEF97':
        case L'\uF1C6':
            return Icon::kRecall;
    }

    return Icon::kUnknown;
}

auto const& S() {
    return settings::g.tray;
}

void Show(FrameworkElement const& e, bool visible) {
    core::Set(e, UIElement::VisibilityProperty(),
              box_value(visible ? Visibility::Visible : Visibility::Collapsed));
}

FrameworkElement Path(FrameworkElement e, std::initializer_list<PCWSTR> names) {
    for (PCWSTR name : names) {
        e = core::ChildNamed(e, name);
    }
    return e;
}

// TextIconContent > ContainerGrid > Base > InnerTextBlock: the glyph.
Controls::TextBlock GlyphOf(FrameworkElement const& textIconContent) {
    return Path(textIconContent, {L"ContainerGrid", L"Base", L"InnerTextBlock"})
        .try_as<Controls::TextBlock>();
}

// ---- main stack: microphone, location, Studio Effects, Recall ----------------

bool HideMain(Icon icon) {
    switch (icon) {
        case Icon::kMicrophone:
            return S().hideMicrophone;
        case Icon::kGeolocation:
            return S().hideLocation;
        case Icon::kMicrophoneAndGeolocation:
            return S().hideMicrophone && S().hideLocation;
        case Icon::kStudioEffects:
            return S().hideStudioEffects;
        case Icon::kRecall:
            return S().hideRecall;
        default:
            return false;  // kNone: the icon is about to go away
    }
}

void MainIcon(FrameworkElement const& iconView) {
    auto glyph = GlyphOf(core::ChildOfClass(
        Path(iconView, {L"ContainerGrid", L"ContentPresenter", L"ContentGrid"}),
        L"SystemTray.TextIconContent"));
    if (!glyph) {
        return;
    }
    Show(iconView, !HideMain(Identify(glyph.Text())));
    // The same view switches between these icons.
    core::Watch(glyph, Controls::TextBlock::TextProperty(),
                [weak = winrt::make_weak(iconView)](DependencyObject const& sender) {
                    if (auto view = weak.get()) {
                        auto text = sender.as<Controls::TextBlock>().Text();
                        Show(view, !HideMain(Identify(text)));
                    }
                });
}

// ---- language bar ---------------------------------------------------------------

void LanguageIcon(FrameworkElement const& iconView) {
    auto grid =
        Path(iconView, {L"ContainerGrid", L"ContentPresenter", L"ContentGrid"});
    std::optional<bool> hide;
    core::FindChild(grid, [&](auto& child) {
        auto cls = winrt::get_class_name(child);
        if (cls == L"SystemTray.LanguageTextIconContent" ||
            cls == L"SystemTray.LanguageImageIconContent") {
            hide = S().hideLanguageBar;
        } else if (cls == L"SystemTray.ImageIconContent") {
            hide = S().hideLanguageExtras;
        } else if (cls == L"SystemTray.TextIconContent") {
            // The input mode shows as a glyph or as a plain letter (가, A).
            hide = S().hideLanguageExtras;
        }
        return hide.has_value();
    });
    if (hide) {
        Show(iconView, !*hide);
    } else if (!Media::VisualTreeHelper::GetChildrenCount(iconView)) {
        // A hidden view left empty when the input method switches between
        // text and image icons stays empty for good unless it's shown.
        Show(iconView, true);
    }
}

// ---- control center: volume, network, battery ---------------------------------

struct Panel {
    winrt::weak_ref<FrameworkElement> panel;
    FrameworkElement::SizeChanged_revoker sizeChanged;
};

std::list<Panel> g_panels;

// A hidden icon keeps its empty item container, and the panel adds Spacing for
// every child. Negative margins on the visible neighbors cancel it, and the
// button goes away when every icon is hidden.
void FixControlCenterSpacing(FrameworkElement const& panel) {
    auto stack = panel.try_as<Controls::StackPanel>();
    double spacing = stack ? stack.Spacing() : 0;
    FrameworkElement pending = nullptr;
    double pendingLeft = 0;
    int hiddenRun = 0;
    auto setMargin = [](FrameworkElement const& container, double l, double r) {
        core::Set(container, FrameworkElement::MarginProperty(),
                  box_value(Thickness{l, 0, r, 0}));
    };
    core::FindChild(panel, [&](auto& container) {
        auto icon = core::ChildNamed(container, L"SystemTrayIcon")
                        .template try_as<Controls::Control>();
        if (!icon) {
            return false;
        }
        if (!icon.IsEnabled()) {
            hiddenRun++;
            return false;
        }
        if (pending) {
            setMargin(pending, pendingLeft, 0);
        }
        pending = container;
        pendingLeft = -spacing * hiddenRun;
        hiddenRun = 0;
        return false;
    });
    if (pending) {
        setMargin(pending, pendingLeft, -spacing * hiddenRun);
    }
    // The button template has width of its own, so an empty panel isn't enough.
    if (auto button = core::AncestorNamed(panel, L"ControlCenterButton")) {
        Show(button, (bool)pending);
    }
}

void ControlCenterIcon(FrameworkElement const& iconView) {
    auto grid = Path(iconView, {L"ContainerGrid", L"ContentGrid"});
    bool hide = false;
    auto content = core::ChildOfClass(grid, L"SystemTray.BatteryIconContent");
    if (content) {
        hide = S().hideBattery;
    } else {
        content = core::ChildOfClass(grid, L"SystemTray.TextIconContent");
        auto glyph = GlyphOf(content);
        if (!glyph) {
            return;
        }
        switch (Identify(glyph.Text())) {
            case Icon::kVolume:
                hide = S().hideVolume;
                break;
            case Icon::kNetwork:
                hide = S().hideNetwork;
                break;
            case Icon::kBattery:
                hide = S().hideBattery;
                break;
            default:
                break;
        }
    }
    Show(content, !hide);
    if (auto control = iconView.try_as<Controls::Control>()) {
        core::Set(control, Controls::Control::IsEnabledProperty(),
                  box_value(!hide));
    }

    // Item container > StackPanel. A removed container changes the panel's
    // size, the only sign that the neighbors' margins are stale.
    auto panel = core::Parent(core::Parent(iconView));
    if (!panel.try_as<Controls::StackPanel>()) {
        return;
    }
    bool known = std::any_of(g_panels.begin(), g_panels.end(),
                             [&](auto& p) { return p.panel.get() == panel; });
    if (!known) {
        g_panels.push_back(
            {winrt::make_weak(panel),
             panel.SizeChanged(winrt::auto_revoke, [](auto&& sender, auto&&) {
                 if (auto panel = sender.template try_as<FrameworkElement>()) {
                     core::Guard(L"Control center",
                                 [&] { FixControlCenterSpacing(panel); });
                 }
             })});
    }
    FixControlCenterSpacing(panel);
}

// ---- notification center: bell and clock ----------------------------------------

std::list<FrameworkElement::LayoutUpdated_revoker> g_bellWaits;

void BellIcon(FrameworkElement const& iconView);

// When the clock is hidden the bell's content is recreated on every change;
// wait for the next layout pass for it to show up.
void WaitForBell(FrameworkElement const& iconView) {
    auto it = g_bellWaits.emplace(g_bellWaits.end());
    *it = iconView.LayoutUpdated(
        winrt::auto_revoke, [it, weak = winrt::make_weak(iconView)](auto&&, auto&&) {
            auto view = weak.get();
            g_bellWaits.erase(it);  // destroys this handler; locals only below
            if (view && !core::g_unloading) {
                core::Guard(L"Bell", [&] { BellIcon(view); });
            }
        });
}

bool HideBell(Icon icon) {
    switch (S().bell) {
        case Bell::kShown:
            return false;
        case Bell::kHidden:
            return true;
        case Bell::kWhenNew:
            return icon == Icon::kBellEmpty || icon == Icon::kBellEmptyDnd;
    }
    return false;
}

// #ContainerGrid > [#ContentPresenter when the clock is hidden] > #ContentGrid >
// SystemTray.TextIconContent
void BellIcon(FrameworkElement const& iconView) {
    auto container = core::ChildNamed(iconView, L"ContainerGrid");
    auto presenter = core::ChildNamed(container, L"ContentPresenter");
    auto content = core::ChildOfClass(
        core::ChildNamed(presenter ? presenter : container, L"ContentGrid"),
        L"SystemTray.TextIconContent");
    if (S().bell == Bell::kWhenNew) {
        core::Watch(iconView, Automation::AutomationProperties::NameProperty(),
                    [](DependencyObject const& sender) {
                        BellIcon(sender.as<FrameworkElement>());
                    });
    }
    if (!content) {
        if (S().bell == Bell::kWhenNew && g_bellWaits.size() < 4) {
            WaitForBell(iconView);
        }
        return;
    }
    auto glyph = GlyphOf(content);
    bool hide = HideBell(glyph ? Identify(glyph.Text()) : Icon::kUnknown);
    Show(content, !hide);
    if (presenter) {
        if (auto item = core::Parent(iconView)) {
            core::Set(item, FrameworkElement::MaxWidthProperty(),
                      box_value(hide ? 0.0
                                     : std::numeric_limits<double>::infinity()));
        }
    }
}

// The time now as tray.clockFormat writes it, from the user's locale: 24 hours, or
// 12 with its AM/PM mark (오전/오후); with seconds when Windows shows them.
std::wstring ClockText(bool seconds) {
    wchar_t locale[80];
    std::wstring format = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT,
                                          seconds ? LOCALE_STIMEFORMAT : LOCALE_SSHORTTIME,
                                          locale, 80)
                              ? locale
                              : (seconds ? L"H:mm:ss" : L"H:mm");
    if (S().clock12) {
        std::ranges::replace(format, L'H', L'h');
        if (format.find(L't') == std::wstring::npos) {
            format = L"tt " + format;
        }
    } else {
        std::erase(format, L't');
        std::ranges::replace(format, L'h', L'H');
        while (format.find(L"  ") != std::wstring::npos) {
            format.erase(format.find(L"  "), 1);
        }
        format.erase(0, format.find_first_not_of(L' '));
        format.erase(format.find_last_not_of(L' ') + 1);
    }
    SYSTEMTIME now;
    GetLocalTime(&now);
    wchar_t text[64];
    return GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &now, format.c_str(), text, 64)
               ? text
               : L"";
}

// Explorer writes the time again each minute; it's written over in the format set.
void FormatClock(Controls::TextBlock const& time) {
    std::wstring shown(time.Text());
    auto text = ClockText(std::ranges::count(shown, L':') >= 2);
    if (!text.empty() && text != shown) {
        time.Text(text);
    }
}

void ClockIcon(FrameworkElement const& iconView) {
    if (S().clock != Clock::kHidden) {
        if (auto time = core::FindDescendant(
                            iconView, [](auto& e) { return e.Name() == L"TimeInnerTextBlock"; },
                            8)
                            .try_as<Controls::TextBlock>()) {
            core::Watch(time, Controls::TextBlock::TextProperty(),
                        [](DependencyObject const& sender) {
                            FormatClock(sender.as<Controls::TextBlock>());
                        });
            FormatClock(time);
        }
    }
    if (S().clock == Clock::kHidden) {
        core::Keep(iconView, UIElement::VisibilityProperty(),
                   box_value(Visibility::Collapsed));
    } else if (S().clock == Clock::kTime) {
        // Explorer shows the date again on every tick.
        if (auto date = core::FindDescendant(
                iconView, [](auto& e) { return e.Name() == L"DateInnerTextBlock"; },
                8)) {
            core::Keep(date, UIElement::VisibilityProperty(),
                       box_value(Visibility::Collapsed));
        }
    }
    // Nothing left in the button: drop it, its template has width of its own.
    if (S().clock == Clock::kHidden && S().bell == Bell::kHidden) {
        if (auto button = core::AncestorNamed(iconView, L"NotificationCenterButton")) {
            Show(button, false);
        }
    }
}

void NotificationIcon(FrameworkElement const& iconView) {
    auto grid = Path(iconView, {L"ContainerGrid", L"ContentGrid"});
    if (core::ChildOfClass(grid, L"SystemTray.DateTimeIconContent")) {
        ClockIcon(iconView);
    } else {
        BellIcon(iconView);
    }
}

// ---- entry points ---------------------------------------------------------------

void OnIcon(FrameworkElement const& icon) {
    auto cls = winrt::get_class_name(icon);
    if (cls == L"SystemTray.CopilotIcon") {
        if (S().hideCopilot) {
            Show(icon, false);
        }
        return;
    }
    if (cls != L"SystemTray.IconView" || icon.Name() != L"SystemTrayIcon") {
        return;
    }
    if (core::AncestorNamed(icon, L"MainStack")) {
        MainIcon(icon);
    } else if (core::AncestorNamed(icon, L"NonActivatableStack")) {
        LanguageIcon(icon);
    } else if (core::AncestorNamed(icon, L"ControlCenterButton")) {
        ControlCenterIcon(icon);
    } else if (core::AncestorNamed(icon, L"NotificationCenterButton")) {
        NotificationIcon(icon);
    }
}

void Apply(HWND hwnd) {
    auto root = core::TaskbarRoot(hwnd);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    core::FindDescendant(
        content,
        [](auto& e) {
            auto cls = winrt::get_class_name(e);
            if (cls == L"SystemTray.IconView" || cls == L"SystemTray.CopilotIcon") {
                core::Guard(L"Tray icon", [&] { OnIcon(e); });
            }
            return false;
        },
        40);
}

void Restore() {
    g_panels.clear();
    g_bellWaits.clear();
}

}  // namespace tray

// ============================================================================
namespace size {
// ============================================================================
// Taskbar height, icon size and button width. The taskbar computes these from
// a few functions and members; the hooks hand it the custom values, and give
// the stock ones back where the taskbar uses them to tell its postures apart.
// Only the paths of Windows 11 builds with dynamic icon scaling (x64) are here.

using winrt::box_value;
using Size = wf::Size;
namespace theme = settings::theme;

std::atomic<bool> g_applying;
std::atomic<bool> g_pendingMeasure;
std::atomic<int> g_hookCalls;
HANDLE g_measured;  // set by the first frame measure after a change

bool g_dynamicIconScaling;
std::atomic<bool> g_smallIconSize;
int g_originalHeight;
int g_height;  // the height the taskbar is given now; 0 before the first apply
int g_iconRoom;  // the height an apply goes to, which the icon fits through its steps
bool g_inTrayUpdateFrameSize;
bool g_buttonWidthCustomized;

// The icon has to fit the bar, inside the pill's edges when the pill is on;
// a larger setting gives the largest that fits.
int IconSize() {
    int size = settings::g.size.iconSize;
    if (int height = g_iconRoom) {
        int inset = 2 * theme::kIconRoom;
        if (settings::g.pill.enabled) {
            // The pill's margins and its one-pixel rim at either edge.
            inset += (int)std::ceil(theme::kAppsMargin.Top + theme::kAppsMargin.Bottom) + 2;
        }
        size = std::min(size, std::max(8, height - inset));
    }
    return size;
}
int ButtonWidth() {
    return IconSize() + settings::g.size.iconSpacing;  // the icon as drawn, so a cut one keeps its spacing
}
bool Vertical() {
    return core::g_vertical;
}

STDAPI GetDpiForMonitor(HMONITOR monitor, int type, UINT* dpiX, UINT* dpiY);

int MonitorDpi(HWND hwnd) {
    UINT dpiX = 0, dpiY = 0;
    if (auto monitor = (HMONITOR)GetProp(hwnd, L"TaskbarMonitor")) {
        GetDpiForMonitor(monitor, 0, &dpiX, &dpiY);
    }
    return dpiY;
}

// ---- machine code lookups -------------------------------------------------------

size_t OffsetFromAssemblyRegex(void* func, std::regex regex, int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; p && i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }
        p += result.length;
        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            return std::stoull(match[1], nullptr, 16);
        }
    }
    Wh_Log(L"Offset not found in %p", func);
    return 0;
}

// The setter of a double member: movsd xmm, [rcx+offset].
size_t MemberOffsetOfSetter(void* setter) {
    size_t offset = OffsetFromAssemblyRegex(
        setter, std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase));
    return offset > 0xFFFF ? 0 : offset;
}

// A consume_general thunk: mov rax, [rcx] / mov rax, [rax+offset].
size_t VcallThunkVtableOffset(void* thunk) {
    static const std::regex regex(R"(mov \w+, \[\w+\+0x([0-9a-f]+)\])",
                                  std::regex_constants::icase);
    return OffsetFromAssemblyRegex(thunk, regex, 2);
}

// IFrameworkElement vtable offsets, past the 6 IInspectable methods.
constexpr size_t kPutWidthOffset = (6 + 10) * sizeof(void*);
constexpr size_t kPutHeightOffset = (6 + 12) * sizeof(void*);

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        unsigned __int32 enabledState : 2;
        unsigned __int32 rest : 26;
        unsigned int payload;
    };
#pragma pack(pop)
    using Query_t = int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static auto query = (Query_t)GetProcAddress(GetModuleHandle(L"ntdll.dll"),
                                                "RtlQueryFeatureConfiguration");
    RTL_FEATURE_CONFIGURATION feature{};
    INT64 changeStamp = 0;
    if (!query || FAILED(query(featureId, 1, &changeStamp, &feature))) {
        return std::nullopt;
    }
    switch (feature.enabledState) {
        case 1:
            return false;
        case 2:
            return true;
    }
    return std::nullopt;
}

// A property accessor of a C++/WinRT interface is a generic "fixed vtable slot"
// thunk the linker folds with the same accessor of unrelated interfaces, so a
// hook on one is called for all of them and has to check the class.
bool IsRuntimeClass(void* pThis, std::wstring_view className) {
    void* abi = pThis ? *(void**)pThis : nullptr;
    if (!abi) {
        return false;
    }
    static std::mutex mutex;
    static std::unordered_map<void*, winrt::hstring> cache;  // by vtable
    std::lock_guard guard(mutex);
    auto [it, inserted] = cache.try_emplace(*(void**)abi);
    if (inserted) {
        core::Guard(L"Class name", [&] {
            it->second = winrt::get_class_name(
                *reinterpret_cast<wf::IInspectable*>(pThis));
        });
    }
    return it->second == className;
}

// Sets a double member for the duration of a call to the original.
template <class F>
void WithMember(void* pThis, size_t offset, double value, F&& call) {
    if (!offset) {
        call();
        return;
    }
    double* member = (double*)((BYTE*)pThis + offset);
    double saved = *member;
    *member = value;
    call();
    *member = saved;
}

double PostureIconHeight() {
    return g_smallIconSize ? 16 : 24;
}

// ---- taskbar.dll ------------------------------------------------------------------

using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis, HMONITOR, SIZE*);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    TrayUI_GetMinSize_Original(pThis, monitor, size);
    // Keeps an auto-hidden secondary taskbar in place.
    if (!Vertical() && g_height) {
        UINT dpiX = 0, dpiY = 0;
        GetDpiForMonitor(monitor, 0, &dpiX, &dpiY);
        size->cy = MulDiv(g_height, dpiY, 96);
    }
}

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t =
    void(WINAPI*)(void* pThis, void*, void*, void*, void*);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* p1,
                                             void* p2,
                                             void* p3,
                                             void* p4) {
    autohide::g_tray = pThis;
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (g_applying) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

// The secondary taskbars ask for their rectangle with ABM_QUERYPOS.
using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
UINT_PTR WINAPI SHAppBarMessage_Hook(DWORD message, PAPPBARDATA data) {
    auto ret = SHAppBarMessage_Original(message, data);
    if (message == ABM_QUERYPOS && ret && !Vertical() && g_height) {
        if (int dpi = MonitorDpi(data->hWnd)) {
            int height = MulDiv(g_height, dpi, 96);
            if (data->uEdge == ABE_TOP) {
                data->rc.bottom = data->rc.top + height;
            } else {
                data->rc.top = data->rc.bottom - height;
            }
        } else {
            Wh_Log(L"No monitor DPI for %08X", (DWORD)(DWORD_PTR)data->hWnd);
        }
    }
    return ret;
}

// ---- Taskbar.View.dll: resources, icon height, frame height ------------------------

void OverrideLookup(wf::IInspectable const* key, wf::IInspectable* value) {
    if (core::g_unloading) {
        return;
    }
    auto name = key->try_as<winrt::hstring>();
    if (!name) {
        return;
    }
    double width;
    if (*name == L"MediumTaskbarButtonExtent") {
        width = ButtonWidth();
    } else if (*name == L"SmallTaskbarButtonExtent") {
        width = theme::kButtonWidthSmall;
    } else {
        return;
    }
    auto old = value->try_as<double>();
    if (old && *old != width) {
        *value = box_value(width);
    }
}

using ResourceDictionary_Lookup_t =
    wf::IInspectable*(WINAPI*)(void* pThis, void** result, wf::IInspectable* key);
ResourceDictionary_Lookup_t ResourceDictionary_Lookup_Original;
wf::IInspectable* WINAPI ResourceDictionary_Lookup_Hook(void* pThis,
                                                       void** result,
                                                       wf::IInspectable* key) {
    auto ret = ResourceDictionary_Lookup_Original(pThis, result, key);
    if (*ret) {
        OverrideLookup(key, ret);
    }
    return ret;
}

// Each taskbar's TaskbarConfiguration tells its buttons the icon height only
// when a Windows setting changes; buttons made before an apply keep the height
// they were made with. The configurations are learned from their icon height
// calls, so an apply can have each tell the new height (RefreshIcons).
using TaskbarConfiguration_Method_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_Method_t TaskbarConfiguration_UpdateIconConfiguration_Original;
TaskbarConfiguration_Method_t TaskbarConfiguration_Destructor_Original;

// Recursive: the refresh comes back into the icon height hook.
std::recursive_mutex g_configurationsLock;
std::vector<void*> g_configurations;

void NoteConfiguration(void* configuration) {
    if (!TaskbarConfiguration_UpdateIconConfiguration_Original ||
        !TaskbarConfiguration_Destructor_Original) {
        return;
    }
    std::lock_guard guard(g_configurationsLock);
    if (std::ranges::find(g_configurations, configuration) == g_configurations.end()) {
        g_configurations.push_back(configuration);
    }
}

void WINAPI TaskbarConfiguration_Destructor_Hook(void* pThis) {
    {
        std::lock_guard guard(g_configurationsLock);
        std::erase(g_configurations, pThis);
    }
    TaskbarConfiguration_Destructor_Original(pThis);
}

// Taskbar thread. Raises IconConfigurationChanged, which hands every button
// and toggle the icon height the hook gives now.
void RefreshIcons() {
    std::lock_guard guard(g_configurationsLock);
    auto list = g_configurations;
    for (void* configuration : list) {
        // One of them may have gone during the previous one's handlers.
        if (std::ranges::find(g_configurations, configuration) != g_configurations.end()) {
            core::Guard(L"Icon configuration", [&] {
                TaskbarConfiguration_UpdateIconConfiguration_Original(configuration);
            });
        }
    }
}

// TaskbarFrame::GetMetrics uses the icon height only to pick a button extent,
// so it gets the stock height, which also tells the GetMetrics hook which one.
thread_local bool g_inGetMetrics;
thread_local std::optional<double> g_getMetricsIconHeight;

using GetIconHeightInViewPixels_t = double(WINAPI*)(void* pThis);
GetIconHeightInViewPixels_t GetIconHeightInViewPixels_Original;
double WINAPI GetIconHeightInViewPixels_Hook(void* pThis) {
    NoteConfiguration(pThis);
    double height = GetIconHeightInViewPixels_Original(pThis);
    // Stock heights tell the postures apart: 16 small, 24 medium, 32 tablet.
    g_smallIconSize = height <= 16;
    if (g_inGetMetrics) {
        g_getMetricsIconHeight = height;
        return height;
    }
    if (!core::g_unloading) {
        return height <= 16 ? std::min(theme::kIconSizeSmall, IconSize()) : IconSize();
    }
    return height;
}

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int taskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int taskbarSize) {
    // 0 small, 1 regular, 2 large.
    bool regular = taskbarSize == 1 || taskbarSize == 2;
    if (!g_originalHeight && regular) {
        g_originalHeight = TaskbarConfiguration_GetFrameSize_Original(taskbarSize);
    }
    if (!Vertical() && g_height && regular) {
        return g_height;
    }
    return TaskbarConfiguration_GetFrameSize_Original(taskbarSize);
}

using TaskbarFrame_SetDouble_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_SetDouble_t TaskbarFrame_MaxHeight_Original;
TaskbarFrame_SetDouble_t TaskbarFrame_Height_Original;
void WINAPI TaskbarFrame_Height_Hook(void* pThis, double value) {
    if (!Vertical() && TaskbarFrame_MaxHeight_Original) {
        TaskbarFrame_MaxHeight_Original(pThis,
                                        std::numeric_limits<double>::infinity());
    }
    TaskbarFrame_Height_Original(pThis, value);
}

void* TaskbarController_OnGroupingModeChanged_Original;

// The frame member of TaskbarController, from the first load in
// OnGroupingModeChanged:
// 48:83EC 28        | sub rsp,28
// 48:8B81 88020000  | mov rax,qword ptr ds:[rcx+288]  (or 4C:8B81, mov r8)
LONG TaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
        auto p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
        Wh_Log(L"Taskbar frame offset not found");
        return 0;
    }();
    return offset;
}

// With custom scaling the DPI is the percentage rounded to a whole DPI (106 for
// 110%). The window height scales by the DPI, XAML by the exact percentage, so
// the frame may leave a gap or get cut off. Returns the height XAML scales to
// the window height.
double HeightMatchingWindow(FrameworkElement const& frame, int height) {
    auto root = frame.XamlRoot();
    if (!root) {
        return height;
    }
    double scale = root.RasterizationScale();
    int windowHeight = MulDiv(height, (int)std::lround(scale * 96), 96);
    if (std::floor(height * scale + 0.5) == windowHeight) {
        return height;
    }
    return windowHeight / scale;
}

bool MatchWindowHeight(FrameworkElement const& frame) {
    int height = g_height;
    if (core::g_unloading || !height || frame.Height() != height || Vertical()) {
        return false;
    }
    double matched = HeightMatchingWindow(frame, height);
    if (matched == height) {
        return false;
    }
    frame.Height(matched);
    return true;
}

void MatchContentGridHeight(FrameworkElement const& frame) {
    if (auto grid = core::Parent(frame)) {
        double height = frame.Height();
        double gridHeight = grid.Height();
        if (gridHeight > 0 && gridHeight != height) {
            grid.Height(height);
        }
    }
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    LONG offset = Vertical() ? 0 : TaskbarFrameOffset();
    void* frameImpl = offset ? *(void**)((BYTE*)pThis + offset) : nullptr;
    auto frame = frameImpl ? core::FromAbi(((IUnknown**)frameImpl)[1]) : nullptr;
    if (!frame) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    core::Guard(L"Frame max height", [&] {
        frame.MaxHeight(std::numeric_limits<double>::infinity());
    });
    TaskbarController_UpdateFrameHeight_Original(pThis);
    core::Guard(L"Content grid height", [&] { MatchContentGridHeight(frame); });
}

using MeasureOverride_t = int(WINAPI*)(void* pThis, Size size, Size* result);
MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(void* pThis, Size size, Size* result) {
    g_hookCalls++;
    // The XAML scale is known once the frame is in the tree, as it is here.
    core::Guard(L"Frame measure", [&] {
        auto frame = core::FromAbi(pThis);
        if (frame && MatchWindowHeight(frame)) {
            MatchContentGridHeight(frame);
        }
    });
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, result);
    if (g_pendingMeasure.exchange(false)) {
        SetEvent(g_measured);
    }
    g_hookCalls--;
    return ret;
}

// The small frame state compares the frame size with the stock 32, which a
// custom height may match by coincidence; the taskbar is told it's regular.
using TaskbarFrame_UpdateIsSmallFrame_t = void(WINAPI*)(void* pThis, bool small);
TaskbarFrame_UpdateIsSmallFrame_t TaskbarFrame_UpdateIsSmallFrame_Original;
void WINAPI TaskbarFrame_UpdateIsSmallFrame_Hook(void* pThis, bool small) {
    TaskbarFrame_UpdateIsSmallFrame_Original(pThis, small && core::g_unloading);
}

// TaskbarFrame keeps the button extents it looked up in OnApplyTemplate. The
// overflow flyout looks them up again and fail-fasts unless the metrics extent
// is one of them, so the metrics get the current extent.
using TaskbarFrame_GetMetrics_t = void*(WINAPI*)(void* pThis, void* metrics);
TaskbarFrame_GetMetrics_t TaskbarFrame_GetMetrics_Original;
void* WINAPI TaskbarFrame_GetMetrics_Hook(void* pThis, void* metrics) {
    g_inGetMetrics = true;
    g_getMetricsIconHeight.reset();
    void* ret = TaskbarFrame_GetMetrics_Original(pThis, metrics);
    g_inGetMetrics = false;
    auto iconHeight = g_getMetricsIconHeight;
    // 32 is the tablet posture, which isn't customized.
    if (!iconHeight || *iconHeight == 32) {
        return ret;
    }
    double width;
    if (*iconHeight == 16) {
        width = core::g_unloading ? 32 : theme::kButtonWidthSmall;
    } else {
        width = core::g_unloading ? 44 : ButtonWidth();
    }
    // The button extent is the second member of TaskbarFrameMetrics.
    double* extent = (double*)((BYTE*)metrics + sizeof(double));
    if (*extent >= 1 && *extent < 10000) {
        *extent = width;
    }
    return ret;
}

// ---- Taskbar.View.dll: task buttons ------------------------------------------------

using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;

size_t IconHeightOffset() {
    static size_t offset =
        TaskListButton_IconHeight_Original
            ? MemberOffsetOfSetter((void*)TaskListButton_IconHeight_Original)
            : 0;
    return offset;
}

// Task button members compare the icon height with the stock 16 and 24 to pick
// paddings, badges and clips; they get the posture height around these calls.
// A badge is a dot at 16, so badges always get 24.
using TaskListButton_Method_t = void(WINAPI*)(void* pThis);
TaskListButton_Method_t TaskListButton_UpdateButtonPadding_Original;
TaskListButton_Method_t TaskListButton_UpdateBadge_Original;
TaskListButton_Method_t TaskListButton_UpdateMultiWindowClip_Original;
TaskListButton_Method_t TaskListButton_CreateMultiWindowClip_Original;

template <TaskListButton_Method_t* original>
void WINAPI TaskListButton_PostureHook(void* pThis) {
    if (!g_dynamicIconScaling || core::g_unloading) {
        (*original)(pThis);
        return;
    }
    double height = original == &TaskListButton_UpdateBadge_Original
                        ? 24
                        : PostureIconHeight();
    WithMember(pThis, IconHeightOffset(), height, [&] { (*original)(pThis); });
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* stream);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* stream) {
    if (!g_dynamicIconScaling || core::g_unloading) {
        TaskListButton_OverlayIcon_Original(pThis, stream);
        return;
    }
    WithMember(pThis, IconHeightOffset(), 24,
               [&] { TaskListButton_OverlayIcon_Original(pThis, stream); });
}

void* TaskListButton_UpdateIconColumnDefinition_Original;

// UpdateIconColumnDefinition loads the medium extent, then subtracts paddings:
// f20f10b648030000  movsd xmm6,[rsi+348h]   (or f2440f10, movsd xmm8+)
// f20f5cb680030000  subsd xmm6,[rsi+380h]
LONG MediumExtentOffset() {
    static LONG offset = []() -> LONG {
        auto start = (const BYTE*)TaskListButton_UpdateIconColumnDefinition_Original;
        LONG candidate = 0;
        for (auto p = start; p && p != start + 0x200; p++) {
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0xC0) == 0x80) {
                candidate = *(LONG*)(p + 4);
            }
            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0xC0) == 0x80) {
                candidate = *(LONG*)(p + 5);
            }
            if ((p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x5C &&
                 (p[3] & 0xC0) == 0x80) ||
                (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x5C &&
                 (p[4] & 0xC0) == 0x80)) {
                return (candidate < 0 || candidate > 0xFFFF) ? 0 : candidate;
            }
        }
        Wh_Log(L"Medium extent offset not found");
        return 0;
    }();
    return offset;
}

// UpdateVisualStates reads the icon height as the posture marker and as the
// progress indicator size: it gets the posture height, and the progress bar
// gets the custom height back (ProgressBar hooks).
thread_local double g_postureIconHeight;
thread_local double g_customIconHeight;

// Existing buttons keep the extents they looked up; they're rewritten here.
void UpdateButtonExtents(void* pThis) {
    LONG offset = MediumExtentOffset();
    if (!offset) {
        return;
    }
    bool changed = false;
    auto update = [&](double* extent, double width) {
        if (*extent >= 1 && *extent < 10000 && *extent != width) {
            *extent = width;
            changed = true;
        }
    };
    double* medium = (double*)((BYTE*)pThis + offset);
    update(medium, core::g_unloading ? 44 : ButtonWidth());
    // The small extent is the member before it.
    update(medium - 1, core::g_unloading ? 32 : theme::kButtonWidthSmall);
    if (changed) {
        g_buttonWidthCustomized = true;
        TaskListButton_PostureHook<&TaskListButton_UpdateButtonPadding_Original>(
            pThis);
    }
}

// Called by the shared UpdateVisualStates hook.
void UpdateVisualStates(void* pThis, void (*original)(void*)) {
    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applying || g_buttonWidthCustomized)) {
        UpdateButtonExtents(pThis);
    }
    size_t offset = IconHeightOffset();
    if (!g_dynamicIconScaling || core::g_unloading || !offset) {
        original(pThis);
        return;
    }
    double* member = (double*)((BYTE*)pThis + offset);
    g_postureIconHeight = PostureIconHeight();
    g_customIconHeight = *member;
    WithMember(pThis, offset, g_postureIconHeight, [&] { original(pThis); });
    g_postureIconHeight = 0;
}

// Other elements get the posture height through the same folded setters, such
// as the running indicator, whose small extent is 16.
bool IsProgressBarPostureWidth(void* pThis, double width) {
    return g_postureIconHeight && width == g_postureIconHeight &&
           IsRuntimeClass(pThis, L"Microsoft.UI.Xaml.Controls.ProgressBar");
}

using ProgressBar_Width_t = void(WINAPI*)(void* pThis, double width);
ProgressBar_Width_t ProgressBar_Width_Original;
void WINAPI ProgressBar_Width_Hook(void* pThis, double width) {
    if (IsProgressBarPostureWidth(pThis, width)) {
        width = g_customIconHeight;
    }
    ProgressBar_Width_Original(pThis, width);
}

using SetDouble_t = void(WINAPI*)(void* pThis, void* method, double* value);
SetDouble_t ProgressBar_SetDouble_Original;
void WINAPI ProgressBar_SetDouble_Hook(void* pThis, void* method, double* value) {
    if (IsProgressBarPostureWidth(pThis, *value) &&
        VcallThunkVtableOffset(method) == kPutWidthOffset) {
        double width = g_customIconHeight;
        ProgressBar_SetDouble_Original(pThis, method, &width);
        return;
    }
    ProgressBar_SetDouble_Original(pThis, method, value);
}

// ---- Taskbar.View.dll: start, task view, widgets -----------------------------------

// Extensions such as the search button size their icons with the height the
// host keeps; only UpdateDefaultWidth uses it as a posture marker.
using TaskbarComponentHost_IconHeight_t = void(WINAPI*)(void* pThis, double);
TaskbarComponentHost_IconHeight_t TaskbarComponentHost_IconHeight_Original;

size_t HostIconHeightOffset() {
    static size_t offset =
        TaskbarComponentHost_IconHeight_Original
            ? MemberOffsetOfSetter((void*)TaskbarComponentHost_IconHeight_Original)
            : 0;
    return offset;
}

using TaskbarComponentHost_UpdateDefaultWidth_t = void(WINAPI*)(void* pThis);
TaskbarComponentHost_UpdateDefaultWidth_t
    TaskbarComponentHost_UpdateDefaultWidth_Original;
void WINAPI TaskbarComponentHost_UpdateDefaultWidth_Hook(void* pThis) {
    if (!g_dynamicIconScaling || core::g_unloading) {
        TaskbarComponentHost_UpdateDefaultWidth_Original(pThis);
        return;
    }
    WithMember(pThis, HostIconHeightOffset(), PostureIconHeight(), [&] {
        TaskbarComponentHost_UpdateDefaultWidth_Original(pThis);
    });
}

using ExperienceToggleButton_IconHeight_get_t = double(WINAPI*)(void* pThis);
ExperienceToggleButton_IconHeight_get_t ExperienceToggleButton_IconHeight_get_Original;
using ExperienceToggleButton_IconHeight_set_t = void(WINAPI*)(void* pThis, double);
ExperienceToggleButton_IconHeight_set_t ExperienceToggleButton_IconHeight_set_Original;

// The setter calls UpdateButtonPadding, which the hook below then skips.
thread_local bool g_inToggleIconHeight;

void SetToggleIconHeight(void* pThis, double height) {
    g_inToggleIconHeight = true;
    ExperienceToggleButton_IconHeight_set_Original(pThis, height);
    g_inToggleIconHeight = false;
}

// The widget content is laid out for the stock icon size; its margins are
// scaled to the custom one.
void UpdateWidgetContent(FrameworkElement const& panel) {
    auto contentGrid = core::ChildNamed(panel, L"AugmentedEntryPointContentGrid");
    if (!contentGrid) {
        return;
    }
    double margin = std::max(0.0, (40.0 - IconSize()) / 2);
    bool unloading = core::g_unloading;
    core::FindChild(contentGrid, [&](auto& child) {
        if (!core::IsClass(child, L"Windows.UI.Xaml.Controls.Grid")) {
            return false;
        }
        auto panelGrid = core::ChildOfClass(child, L"Windows.UI.Xaml.Controls.Grid");
        auto items = core::ChildOfClass(panelGrid,
                                        L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!items) {
            return false;
        }
        double labelsExtraTop = 0;
        // Right and bottom stay 0: tighter values make the icon disappear at
        // times (windhawk-mods issue 726).
        if (panelGrid.Width() > panelGrid.Height()) {
            Thickness m{3, 3, 3, 3};
            if (!unloading && margin <= 3) {
                labelsExtraTop = 3 - margin;
                m = {margin, margin, 0, 0};
            }
            items.Margin(m);
            panelGrid.VerticalAlignment(unloading ? VerticalAlignment::Stretch
                                                  : VerticalAlignment::Center);
        } else {
            Thickness m{8, 8, 8, 8};
            if (!unloading) {
                double top = margin;
                if (g_height < 48) {
                    top = std::max(0.0, top - (48.0 - g_height) / 2);
                }
                m = {margin, top, 0, 0};
            }
            items.Margin(m);
        }

        auto ticker = core::ChildOfClass(
            core::ChildOfClass(core::ChildOfClass(items, L"Windows.UI.Xaml.Controls.Border"),
                               L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel"),
            L"Windows.UI.Xaml.Controls.Grid");
        if (!ticker) {
            return false;
        }
        double badgeMax = unloading ? 24 : 40 - margin * 2;
        for (auto [ticker1, anchor] :
             {std::pair{L"SmallTicker1", L"BadgeAnchorSmallTicker"},
              std::pair{L"LargeTicker1", L"BadgeAnchorLargeTicker"}}) {
            if (auto badge = core::ChildNamed(
                    core::ChildOfClass(core::ChildNamed(ticker, ticker1),
                                       L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel"),
                    anchor)) {
                badge.MaxWidth(badgeMax);
                badge.MaxHeight(badgeMax);
            }
        }
        if (auto labels = core::ChildNamed(ticker, L"LargeTicker2")) {
            labels.Margin({0, labelsExtraTop, 0, 0});
        }
        return false;
    });
}

void UpdateToggleButtonWidth(void* pThis) {
    auto button = core::FromAbi(((IUnknown**)pThis)[1]);
    auto panel = core::ChildNamed(button, L"ExperienceToggleButtonRootPanel")
                     .try_as<Controls::Grid>();
    if (!panel) {
        return;
    }
    auto cls = winrt::get_class_name(button);
    // AugmentedEntryPointButton::UpdateButtonPadding tail calls this one.
    if (cls == L"Taskbar.AugmentedEntryPointButton") {
        UpdateWidgetContent(panel);
        return;
    }
    if (core::g_unloading) {
        return;
    }
    double extra = -4;
    if (cls == L"Taskbar.ExperienceToggleButton") {
        if (Automation::AutomationProperties::GetAutomationId(button) ==
            L"StartButton") {
            extra = -3;
        }
    } else if (cls == L"Taskbar.SearchBoxButton") {
        if (panel.Margin() != Thickness{}) {
            return;  // a search box, not the icon
        }
    } else {
        return;
    }
    double width = panel.Width();
    if (!(width > 0)) {
        return;
    }
    // The start button's padding depends on the taskbar alignment.
    auto padding = panel.Padding();
    double button_ = g_smallIconSize ? theme::kButtonWidthSmall : ButtonWidth();
    double newWidth = button_ + padding.Left + padding.Right + extra;
    if (newWidth != width) {
        panel.Width(newWidth);
    }
}

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
// Set by CollectTaskbarView: the hook below also serves the start feature,
// which leaves the button sizes alone.
bool g_sizesToggles = false;
// The start feature's, between its Apply and Restore: a toggle button that
// has (maybe again) its template.
void (*g_onToggleButton)(FrameworkElement const&) = nullptr;

void SizeToggleButton(void* pThis);

void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (g_inToggleIconHeight) {
        return;
    }
    if (!g_sizesToggles) {
        ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    } else {
        SizeToggleButton(pThis);
    }
    if (g_onToggleButton && !core::g_unloading) {
        core::Guard(L"Start button", [&] {
            g_onToggleButton(core::FromAbi(((IUnknown**)pThis)[1]));
        });
    }
}

void SizeToggleButton(void* pThis) {
    // The extent and padding compare the icon height with the stock 16 and 32,
    // so the call gets the posture height; the setter also sizes the icon
    // element, hence setting it back right after.
    std::optional<double> saved;
    if (g_dynamicIconScaling && !core::g_unloading &&
        ExperienceToggleButton_IconHeight_get_Original &&
        ExperienceToggleButton_IconHeight_set_Original) {
        double height = ExperienceToggleButton_IconHeight_get_Original(pThis);
        if (height != PostureIconHeight()) {
            saved = height;
            SetToggleIconHeight(pThis, PostureIconHeight());
        }
    }
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (saved) {
        SetToggleIconHeight(pThis, *saved);
    }
    core::Guard(L"Toggle button width", [&] { UpdateToggleButtonWidth(pThis); });
}

void CollectToggleButton(std::vector<WindhawkUtils::SYMBOL_HOOK>& list) {
    list.push_back({{LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
                    &ExperienceToggleButton_UpdateButtonPadding_Original,
                    ExperienceToggleButton_UpdateButtonPadding_Hook});
}

// ---- SystemTray.dll -----------------------------------------------------------------

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t SystemTrayController_UpdateFrameSize_Address;
SystemTrayController_UpdateFrameSize_t SystemTrayController_UpdateFrameSize_Original;

// The last height member, which UpdateFrameSize compares the new height with:
// 66 0f 2e b3 b0 00 00 00  ucomisd xmm6,[rbx+0xb0]
// 7a 4c / 7a 02 / 7a 06    jp
// 75 4a / 74 10 / 0f 84 .. jnz, je
LONG LastHeightOffset() {
    static LONG offset = []() -> LONG {
        auto start = (const BYTE*)SystemTrayController_UpdateFrameSize_Address;
        for (auto p = start; p && p != start + 0x400; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E &&
                (p[3] & 0xC0) == 0x80 && p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG offset = *(LONG*)(p + 4);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
        Wh_Log(L"Last height offset not found");
        return 0;
    }();
    return offset;
}

void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    if (Vertical()) {
        SystemTrayController_UpdateFrameSize_Original(pThis);
        return;
    }
    // Forgets the last height so the frame size is set again.
    if (LONG offset = LastHeightOffset()) {
        *(double*)((BYTE*)pThis + offset) = 0;
    }
    g_inTrayUpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

using SystemTraySecondaryController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

// The tray height is set explicitly, or it may not match the custom height.
using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (!Vertical() && g_inTrayUpdateFrameSize && g_height) {
        value = g_height;
    }
    SystemTrayFrame_Height_Original(pThis, value);
}

SetDouble_t SystemTrayFrame_SetDouble_Original;
void WINAPI SystemTrayFrame_SetDouble_Hook(void* pThis, void* method, double* value) {
    if (g_inTrayUpdateFrameSize && g_height && !Vertical() &&
        VcallThunkVtableOffset(method) == kPutHeightOffset) {
        double height = g_height;
        SystemTrayFrame_SetDouble_Original(pThis, method, &height);
        return;
    }
    SystemTrayFrame_SetDouble_Original(pThis, method, value);
}

// The omni button style and the show desktop column width compare the frame
// height with the stock 24, 32 and 72; collapsed omni buttons take no input.
// These checks, and only they, get the stock height.
thread_local bool g_inTrayModeCheck;

using SystemTrayFrame_GetStyle_t = int(WINAPI*)(void* pThis, void** style);
SystemTrayFrame_GetStyle_t SystemTrayFrame_GetStyle_Original;
int WINAPI SystemTrayFrame_GetStyle_Hook(void* pThis, void** style) {
    g_inTrayModeCheck = true;
    int ret = SystemTrayFrame_GetStyle_Original(pThis, style);
    g_inTrayModeCheck = false;
    return ret;
}

using SystemTrayFrame_GetShowDesktopButtonColumnWidth_t =
    int(WINAPI*)(void* pThis, double* width);
SystemTrayFrame_GetShowDesktopButtonColumnWidth_t
    SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original;
int WINAPI SystemTrayFrame_GetShowDesktopButtonColumnWidth_Hook(void* pThis,
                                                                double* width) {
    g_inTrayModeCheck = true;
    int ret = SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original(pThis, width);
    g_inTrayModeCheck = false;
    return ret;
}

using SystemTrayFrame_Height_get_t = double(WINAPI*)(void* pThis);
SystemTrayFrame_Height_get_t SystemTrayFrame_Height_get_Original;
double WINAPI SystemTrayFrame_Height_get_Hook(void* pThis) {
    if (g_inTrayModeCheck && !Vertical() && g_originalHeight) {
        return g_originalHeight;
    }
    return SystemTrayFrame_Height_get_Original(pThis);
}

// The tray takes its mode from the extent it's measured with, so a custom
// height equal to a stock one (24, 32, 72) would give it that mode. It gets
// the stock extent; the elements it lays out are measured with the real one.
thread_local Size* g_trayMeasureSize;

using FrameworkElementOverrides_MeasureOverride_t =
    Size*(WINAPI*)(void* pThis, Size* result, Size* available);
FrameworkElementOverrides_MeasureOverride_t
    FrameworkElementOverrides_MeasureOverride_Original;
Size* WINAPI FrameworkElementOverrides_MeasureOverride_Hook(void* pThis,
                                                           Size* result,
                                                           Size* available) {
    // The tray frame calls this once, right away; consume the size so it
    // doesn't reach the elements measured further down.
    if (g_trayMeasureSize) {
        available = g_trayMeasureSize;
        g_trayMeasureSize = nullptr;
    }
    return FrameworkElementOverrides_MeasureOverride_Original(pThis, result,
                                                              available);
}

MeasureOverride_t SystemTrayFrame_MeasureOverride_Original;
int WINAPI SystemTrayFrame_MeasureOverride_Hook(void* pThis, Size size, Size* result) {
    core::Guard(L"Tray measure", [&] {
        if (auto frame = core::FromAbi(pThis)) {
            MatchWindowHeight(frame);
        }
    });
    if (!g_originalHeight || !FrameworkElementOverrides_MeasureOverride_Original ||
        Vertical()) {
        return SystemTrayFrame_MeasureOverride_Original(pThis, size, result);
    }
    Size available = size;
    size.Height = (float)g_originalHeight;
    g_trayMeasureSize = &available;
    int ret = SystemTrayFrame_MeasureOverride_Original(pThis, size, result);
    g_trayMeasureSize = nullptr;
    return ret;
}

// ---- registration -------------------------------------------------------------------

using SH = WindhawkUtils::SYMBOL_HOOK;

void CollectTaskbarDll(std::vector<SH>& list) {
    list.push_back({{LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
                    &TrayUI_GetMinSize_Original, TrayUI_GetMinSize_Hook, true});
    list.push_back({{LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
                    &TrayUI__StuckTrayChange_Original});
    list.push_back({{LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
                    &TrayUI__HandleSettingChange_Original,
                    TrayUI__HandleSettingChange_Hook});
}

void CollectTaskbarView(std::vector<SH>& list) {
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                     LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )"},
                    &ResourceDictionary_Lookup_Original,
                    ResourceDictionary_Lookup_Hook});
    list.push_back({{LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
                    &GetIconHeightInViewPixels_Original,
                    GetIconHeightInViewPixels_Hook, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateIconConfiguration(void))"},
                    &TaskbarConfiguration_UpdateIconConfiguration_Original, nullptr, true});
    list.push_back({{LR"(public: virtual __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::~TaskbarConfiguration(void))"},
                    &TaskbarConfiguration_Destructor_Original,
                    TaskbarConfiguration_Destructor_Hook, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
                    &TaskListButton_IconHeight_Original, nullptr, true});
    list.push_back({{LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                    &TaskbarConfiguration_GetFrameSize_Original,
                    TaskbarConfiguration_GetFrameSize_Hook, true});
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
                    &TaskbarFrame_MaxHeight_Original, nullptr, true});
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                     LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )"},
                    &TaskbarFrame_Height_Original, TaskbarFrame_Height_Hook, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
                    &TaskbarController_OnGroupingModeChanged_Original, nullptr, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                    &TaskbarController_UpdateFrameHeight_Original,
                    TaskbarController_UpdateFrameHeight_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                    &TaskbarFrame_MeasureOverride_Original,
                    TaskbarFrame_MeasureOverride_Hook});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::UpdateIsSmallFrame(bool))"},
                    &TaskbarFrame_UpdateIsSmallFrame_Original,
                    TaskbarFrame_UpdateIsSmallFrame_Hook, true});
    list.push_back({{LR"(public: struct winrt::Taskbar::implementation::TaskbarFrameMetrics __cdecl winrt::Taskbar::implementation::TaskbarFrame::GetMetrics(void)const )"},
                    &TaskbarFrame_GetMetrics_Original, TaskbarFrame_GetMetrics_Hook,
                    true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                    &TaskListButton_UpdateButtonPadding_Original,
                    TaskListButton_PostureHook<&TaskListButton_UpdateButtonPadding_Original>});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
                    &TaskListButton_OverlayIcon_Original,
                    TaskListButton_OverlayIcon_Hook});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
                    &TaskListButton_UpdateBadge_Original,
                    TaskListButton_PostureHook<&TaskListButton_UpdateBadge_Original>});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
                    &TaskListButton_UpdateMultiWindowClip_Original,
                    TaskListButton_PostureHook<&TaskListButton_UpdateMultiWindowClip_Original>,
                    true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
                    &TaskListButton_CreateMultiWindowClip_Original,
                    TaskListButton_PostureHook<&TaskListButton_CreateMultiWindowClip_Original>,
                    true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
                    &TaskListButton_UpdateIconColumnDefinition_Original, nullptr,
                    true});
    list.push_back({{LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
                    &TaskbarComponentHost_IconHeight_Original, nullptr, true});
    list.push_back({{LR"(private: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::UpdateDefaultWidth(void))"},
                    &TaskbarComponentHost_UpdateDefaultWidth_Original,
                    TaskbarComponentHost_UpdateDefaultWidth_Hook, true});
    list.push_back({{LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
                    &ExperienceToggleButton_IconHeight_get_Original, nullptr, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
                    &ExperienceToggleButton_IconHeight_set_Original, nullptr, true});
    g_sizesToggles = true;
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar>::Width(double)const )"},
                    &ProgressBar_Width_Original, ProgressBar_Width_Hook, true});
    list.push_back({{LR"(void __cdecl winrt::impl::consume_general<struct winrt::Windows::UI::Xaml::IFrameworkElement,struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &>(struct winrt::Microsoft::UI::Xaml::Controls::ProgressBar const *,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &))"},
                    &ProgressBar_SetDouble_Original, ProgressBar_SetDouble_Hook, true});
}

void CollectSystemTray(std::vector<SH>& list) {
    // Hooked by hand once resolved: the offset lookup needs its real address.
    list.push_back({{LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
                    &SystemTrayController_UpdateFrameSize_Address, nullptr, true});
    list.push_back({{LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
                    &SystemTraySecondaryController_UpdateFrameSize_Original,
                    SystemTraySecondaryController_UpdateFrameSize_Hook, true});
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
                    &SystemTrayFrame_Height_Original, SystemTrayFrame_Height_Hook,
                    true});
    list.push_back({{LR"(void __cdecl winrt::impl::consume_general<struct winrt::Windows::UI::Xaml::IFrameworkElement,struct winrt::SystemTray::SystemTrayFrame,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &>(struct winrt::SystemTray::SystemTrayFrame const *,int (__cdecl winrt::impl::abi<struct winrt::Windows::UI::Xaml::IFrameworkElement,void>::type::*)(double) noexcept,double &))"},
                    &SystemTrayFrame_SetDouble_Original, SystemTrayFrame_SetDouble_Hook,
                    true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetStyle(void * *))"},
                    &SystemTrayFrame_GetStyle_Original, SystemTrayFrame_GetStyle_Hook,
                    true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::SystemTray::ISystemTrayFrame>::GetShowDesktopButtonColumnWidth(double *))"},
                    &SystemTrayFrame_GetShowDesktopButtonColumnWidth_Original,
                    SystemTrayFrame_GetShowDesktopButtonColumnWidth_Hook, true});
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::implementation::SystemTrayFrame>::Height(void)const )"},
                    &SystemTrayFrame_Height_get_Original,
                    SystemTrayFrame_Height_get_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::SystemTrayFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                    &SystemTrayFrame_MeasureOverride_Original,
                    SystemTrayFrame_MeasureOverride_Hook, true});
    list.push_back({{LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElementOverrides<struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size const &)const )"},
                    &FrameworkElementOverrides_MeasureOverride_Original,
                    FrameworkElementOverrides_MeasureOverride_Hook, true});
}

// After Taskbar.View.dll's symbols resolved.
void TaskbarViewHooked() {
    IconHeightOffset();
    HostIconHeightOffset();
    TaskbarFrameOffset();
    MediumExtentOffset();
    constexpr UINT kDynamicIconScaling = 29785184;
    g_dynamicIconScaling = GetIconHeightInViewPixels_Original &&
                           IsOsFeatureEnabled(kDynamicIconScaling).value_or(true);
    // shortcut: without dynamic icon scaling (older builds) icons keep the
    // stock size; add the IconUtils/GetIconHeight hooks of taskbar-icon-size
    // to support them.
    Wh_Log(L"Dynamic icon scaling: %d", g_dynamicIconScaling);
}

// After SystemTray.dll's symbols resolved.
void SystemTrayHooked() {
    if (SystemTrayController_UpdateFrameSize_Address) {
        LastHeightOffset();
        WindhawkUtils::SetFunctionHook(SystemTrayController_UpdateFrameSize_Address,
                                       SystemTrayController_UpdateFrameSize_Hook,
                                       &SystemTrayController_UpdateFrameSize_Original);
    }
}

void InstallWin32() {
    g_measured = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    WindhawkUtils::SetFunctionHook(SHAppBarMessage, SHAppBarMessage_Hook,
                                   &SHAppBarMessage_Original);
}

// ---- applying -------------------------------------------------------------------------

// Asks the taskbar to recompute its size and waits for the frame to be measured
// with it. Not on the taskbar thread: the measure happens there.
void Trigger(HWND taskbar) {
    g_pendingMeasure = true;
    ResetEvent(g_measured);
    // TrayUI::_HandleSettingChange, which then calls _StuckTrayChange.
    SendMessage(taskbar, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    if (!Vertical() && WaitForSingleObject(g_measured, 10000) != WAIT_OBJECT_0) {
        Wh_Log(L"No measure after the size change");
    }
    g_pendingMeasure = false;
}

void Apply(int height) {
    height = std::max(height, 2);
    g_iconRoom = height;
    HWND taskbar = core::PrimaryTaskbar();
    if (!taskbar) {
        g_height = height;
        return;
    }
    if (!g_height) {
        g_height = g_originalHeight;
    }
    if (!g_height) {
        RECT rect{};
        GetWindowRect(taskbar, &rect);
        int dpi = MonitorDpi(taskbar);
        g_height = MulDiv(rect.bottom - rect.top, 96, dpi ? dpi : 96);
    }

    g_applying = true;
    // The same height again doesn't refresh; go through another one first.
    if (!Vertical() && height == g_height) {
        g_height = height - 1;
        Trigger(taskbar);
    }
    g_height = height;
    Trigger(taskbar);
    // The buttons that exist take the icon height only when told.
    core::RunOn(taskbar, RefreshIcons);

    // CTaskBand::_HandleSyncDisplayChange reloads the icons.
    HWND rebar = FindWindowEx(taskbar, nullptr, L"ReBarWindow32", nullptr);
    if (HWND band = rebar ? FindWindowEx(rebar, nullptr, L"MSTaskSwWClass", nullptr)
                          : nullptr) {
        SendMessage(band, 0x452, 3, 0);
    }
    g_applying = false;
}

void ApplySettings() {
    Apply(settings::g.size.taskbarHeight);
}

void Unapply() {
    Apply(g_originalHeight ? g_originalHeight : 48);
}

// Hooks still running when the mod unloads.
void WaitForHooks() {
    while (g_hookCalls > 0) {
        Sleep(100);
    }
}

}  // namespace size

namespace start {
bool LauncherAhead();
bool IsLauncher(HWND hwnd);
}  // namespace start

namespace panels {
bool IsNotificationCenter(HWND hwnd);
}  // namespace panels

// ============================================================================
namespace autohide {
// ============================================================================
// Auto-hide itself stays a Windows setting. This lets the native top taskbar
// auto-hide, limits where along the edge the pointer brings it back (to the
// island while that shows), replaces Windows' hover delay (the mouse hover
// time, used both ways) with the set delays, and sets the slide animation
// length. Pointer requests are caught
// where the taskbar decides to expand or collapse; a delayed one waits on a
// one-shot timer that the next request replaces or cancels.

using SH = WindhawkUtils::SYMBOL_HOOK;
namespace wuc = winrt::Windows::UI::Composition;

// TaskbarExpandCollapseReason: the pointer over the taskbar frame changed, and
// the pointer entered the screen edge.
constexpr int kReasonPointerOver = 7;
constexpr int kReasonScreenEdge = 8;
// Our timer on the taskbar window; explorer's own ids there are small.
constexpr UINT_PTR kTimerId = 0x50414854;

bool Active() {
    return !core::g_unloading && !core::g_vertical;
}

// ---- taskbar.dll ------------------------------------------------------------------

// While this feature is on, the native primary taskbar doesn't auto-hide off
// the bottom edge; the rest of auto-hide handles every edge.
using WilFeature_IsEnabled_t = bool(WINAPI*)(void* pThis);
WilFeature_IsEnabled_t WilFeature_61970341_IsEnabled_Original;
bool WINAPI WilFeature_61970341_IsEnabled_Hook(void* pThis) {
    return Active() ? false : WilFeature_61970341_IsEnabled_Original(pThis);
}

// ---- Taskbar.View.dll: expand and collapse ----------------------------------------

using UpdateIsExpanded_t = void(WINAPI*)(void* pThis, HWND hwnd, int reason);
UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;
using UpdateIsExpandedAfterHoverDelay_t =
    void(WINAPI*)(void* pThis, HWND hwnd, bool isPointerOver, int reason);
UpdateIsExpandedAfterHoverDelay_t
    ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Original;
using IsExpanded_t = bool(WINAPI*)(void* pThis, HWND hwnd);
IsExpanded_t ViewCoordinator_IsExpanded_Original;
// The second argument is the current expanded state.
using ShouldTaskbarBeExpanded_t = bool(WINAPI*)(void* pThis, HWND hwnd, bool);
ShouldTaskbarBeExpanded_t ViewCoordinator_ShouldTaskbarBeExpanded_Original;
using HandleIsExpandedChanged_t = void(WINAPI*)(void* pThis, HWND hwnd);
HandleIsExpandedChanged_t ViewCoordinator_HandleIsExpandedChanged_Original;

struct Pending {
    void* coordinator;
    int reason;
};

// Taskbar thread only.
std::unordered_map<HWND, Pending> g_pending;
std::unordered_map<HWND, bool> g_collapsed;
// Out when the mod started, for the tray's hide check once the apply is done.
[[clang::no_destroy]] std::vector<HWND> g_outAtApply;
// For the island (F8), set between its Apply and Restore: told when a
// taskbar's collapsed state changed, and, while it shows, asked whether the
// pointer at the edge at screen x may bring the taskbar out (over an island).
void (*g_onCollapsedChanged)(HWND) = nullptr;
bool (*g_revealsAt)(HWND, int x, bool& reveals) = nullptr;
// Also the island's: told as the taskbar starts to show itself expanded or
// collapsed, before it draws that, and asked whether it stands in for the
// taskbar's slide.
void (*g_onExpandedChanged)(HWND, bool expanded) = nullptr;
bool (*g_standsIn)(HWND) = nullptr;
// Also the island's, in its click mode: true while the pointer on a hidden
// taskbar's edge strip must not bring it out.
bool (*g_holdsHover)(HWND) = nullptr;
// The expand about to play is the island's to show; the taskbar's own slide
// then takes no time, under it.
bool g_expandUnderIsland;

bool Customized() {
    auto& s = settings::g.autohide;
    return s.revealZone < 100 || s.showDelay >= 0 || s.hideDelay >= 0 ||
           g_revealsAt;
}

bool Ready() {
    return Active() && Customized() && ViewCoordinator_UpdateIsExpanded_Original &&
           ViewCoordinator_IsExpanded_Original &&
           ViewCoordinator_ShouldTaskbarBeExpanded_Original;
}

bool IsPointerReason(int reason) {
    return reason == kReasonPointerOver || reason == kReasonScreenEdge;
}

// A program's window that sits over a hidden taskbar's edge or its island
// (Bloom's screen-recording bar grows out of them) asks with this window
// property, on its top-level window, that the pointer on it doesn't bring the
// taskbar out over it. The ask ends with the window or the property.
constexpr wchar_t kHoldRevealProp[] = L"PillTaskbar.HoldReveal";

bool PointerOnHolder() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }
    HWND h = WindowFromPoint(pt);
    h = h ? GetAncestor(h, GA_ROOT) : nullptr;
    return h && GetPropW(h, kHoldRevealProp);
}

// A pointer request to bring a hidden taskbar out while the pointer is on such
// a window: dropped.
bool HeldByWindow(void* coordinator, HWND hwnd, int reason) {
    return IsPointerReason(reason) && ViewCoordinator_IsExpanded_Original &&
           !ViewCoordinator_IsExpanded_Original(coordinator, hwnd) && PointerOnHolder();
}

// True if the pointer asking to show the taskbar is beside the reveal zone:
// the island while it shows, else the set part of the edge, around the middle
// or from the side the pill is set against.
bool OutsideZone(HWND hwnd, int reason) {
    int zone = settings::g.autohide.revealZone;
    RECT rc;
    if (!IsPointerReason(reason) || !GetWindowRect(hwnd, &rc)) {
        return false;
    }
    DWORD pos = GetMessagePos();
    POINT pt{(short)LOWORD(pos), (short)HIWORD(pos)};
    // A show the pointer didn't ask for (it's elsewhere) isn't filtered.
    if (reason == kReasonPointerOver && !PtInRect(&rc, pt)) {
        return false;
    }
    bool reveals;
    if (g_revealsAt && g_revealsAt(hwnd, pt.x, reveals)) {
        return !reveals;
    }
    if (zone >= 100) {
        return false;
    }
    int width = rc.right - rc.left;
    switch (settings::PillAlign()) {
        case settings::Align::kLeft:
            return pt.x >= rc.right - MulDiv(width, 100 - zone, 100);
        case settings::Align::kRight:
            return pt.x < rc.left + MulDiv(width, 100 - zone, 100);
        default: {
            int margin = MulDiv(width, 100 - zone, 200);
            return pt.x < rc.left + margin || pt.x >= rc.right - margin;
        }
    }
}

void Cancel(HWND hwnd) {
    if (g_pending.erase(hwnd)) {
        KillTimer(hwnd, kTimerId);
    }
}

// The region code clips only a shown taskbar: a hidden one keeps just a strip
// on the screen, which must stay for the pointer.
bool Collapsed(HWND hwnd) {
    if (core::g_vertical) {
        return false;
    }
    auto it = g_collapsed.find(hwnd);
    return it != g_collapsed.end() && it->second;
}

bool LooksHidden(HWND taskbar);
void KeepHeldOut(HWND hwnd);
void CloseCenterWith(HWND hwnd);

void SetCollapsed(HWND hwnd, bool collapsed) {
    auto [it, added] = g_collapsed.try_emplace(hwnd, collapsed);
    if (added || it->second != collapsed) {
        RECT rc{};
        GetWindowRect(hwnd, &rc);
        core::DiagLine(L"SetCollapsed %p %d (added %d) rect %d..%d", hwnd, collapsed, added,
                       rc.top, rc.bottom);
        it->second = collapsed;
        // Outside explorer's expand/collapse call.
        core::Post([hwnd] {
            region::OnWindowMoved(hwnd);
            if (g_onCollapsedChanged) {
                g_onCollapsedChanged(hwnd);
            }
        });
        if (collapsed) {
            KeepHeldOut(hwnd);
            CloseCenterWith(hwnd);
        }
    }
}

// The state isn't taken from the view coordinator: on this build it says the
// taskbar is expanded all along. The window's move tells.
// A hidden top taskbar's window sits above the monitor.
void NoteWindowPos(HWND hwnd) {
    if (!core::g_unloading && !core::g_vertical) {
        SetCollapsed(hwnd, LooksHidden(hwnd));
    }
}


void CALLBACK OnTimer(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hwnd, id);
    auto it = g_pending.find(hwnd);
    if (it == g_pending.end()) {
        return;
    }
    Pending p = it->second;
    g_pending.erase(it);
    if (!Ready()) {
        return;
    }
    core::Guard(L"Auto-hide timer", [&] {
        bool expanded = ViewCoordinator_IsExpanded_Original(p.coordinator, hwnd);
        if (!expanded && OutsideZone(hwnd, p.reason)) {
            return;
        }
        ViewCoordinator_UpdateIsExpanded_Original(p.coordinator, hwnd, p.reason);
    });
}

// Takes a pointer request to expand or collapse. True if handled here (run
// now, delayed, or dropped outside the reveal zone); false leaves it to
// explorer's own timing.
bool Decide(void* coordinator, HWND hwnd, int reason) {
    bool expanded = ViewCoordinator_IsExpanded_Original(coordinator, hwnd);
    if (ViewCoordinator_ShouldTaskbarBeExpanded_Original(coordinator, hwnd,
                                                         expanded) == expanded) {
        Cancel(hwnd);  // e.g. the pointer came back before the hide
        return false;
    }
    bool show = !expanded;
    if (show && OutsideZone(hwnd, reason)) {
        Cancel(hwnd);
        return true;
    }
    auto& s = settings::g.autohide;
    int delay = show ? s.showDelay : s.hideDelay;
    if (delay < 0) {
        Cancel(hwnd);
        return false;
    }
    if (delay > 0) {
        g_pending[hwnd] = {coordinator, reason};
        // Replaces a pending timer of this window.
        if (SetTimer(hwnd, kTimerId, delay, OnTimer)) {
            return true;
        }
        g_pending.erase(hwnd);
        Wh_Log(L"Auto-hide timer failed");
    } else {
        Cancel(hwnd);
    }
    ViewCoordinator_UpdateIsExpanded_Original(coordinator, hwnd, reason);
    return true;
}

void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(void* pThis,
                                                  HWND hwnd,
                                                  int reason) {
    bool handled = false;
    if (Active()) {
        core::Guard(L"Expand hold", [&] { handled = HeldByWindow(pThis, hwnd, reason); });
    }
    if (!handled && IsPointerReason(reason) && Ready()) {
        core::Guard(L"Expand request",
                    [&] { handled = Decide(pThis, hwnd, reason); });
    }
    if (!handled) {
        ViewCoordinator_UpdateIsExpanded_Original(pThis, hwnd, reason);
    }
}

// Windows waits the mouse hover time here, both ways, before UpdateIsExpanded.
void WINAPI ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Hook(
    void* pThis,
    HWND hwnd,
    bool isPointerOver,
    int reason) {
    bool handled = false;
    if (Active()) {
        core::Guard(L"Hover hold", [&] { handled = HeldByWindow(pThis, hwnd, reason); });
    }
    if (!handled && IsPointerReason(reason) && Ready()) {
        core::Guard(L"Hover request",
                    [&] { handled = Decide(pThis, hwnd, reason); });
    }
    if (!handled) {
        ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Original(
            pThis, hwnd, isPointerOver, reason);
    }
}

struct Animation;
void SetDuration(Animation& a, bool stock);
extern Animation g_expand;

// TaskbarController::OnIsExpandedChanged calls this before it plays the slide
// of the new state, which is the moment the island takes over.
void WINAPI ViewCoordinator_HandleIsExpandedChanged_Hook(void* pThis, HWND hwnd) {
    ViewCoordinator_HandleIsExpandedChanged_Original(pThis, hwnd);
    core::Guard(L"Expanded changed", [&] {
        if (!Active() || !ViewCoordinator_IsExpanded_Original) {
            return;
        }
        bool expanded = ViewCoordinator_IsExpanded_Original(pThis, hwnd);
        bool under = expanded && g_standsIn && g_standsIn(hwnd);
        core::DiagLine(L"ExpandedChanged %p expanded %d under %d", hwnd, expanded, under);
        if (under != g_expandUnderIsland) {
            g_expandUnderIsland = under;
            SetDuration(g_expand, false);
        }
        if (g_onExpandedChanged) {
            g_onExpandedChanged(hwnd, expanded);
        }
    });
}

// ---- Taskbar.dll: the trays -------------------------------------------------------
// On this build the legacy trays move a top taskbar's window in and out (TrayUI
// the primary's, CSecondaryTray each other one's). The mod asks them only as
// their own hover timers do, through pointers checked against Taskbar.dll's
// own before each call, on the taskbar thread.

// Taskbar.dll 10.0.26100, from TrayUI::TrayUI: where TrayUI keeps ITrayUI
// (what g_trayUI holds) and ITrayComponentHost (TrayUI::Unhide's this), the
// GetAutoHideFlags slot of ITrayUI, and the field after ITrayComponentHost
// that TrayUI::Unhide reads unchecked, set once the tray is initialized.
constexpr ptrdiff_t kTrayUIInterface = 0x28;
constexpr ptrdiff_t kTrayUIHost = 0x30;
constexpr int kGetAutoHideFlagsSlot = 0x90 / 8;
constexpr ptrdiff_t kTrayUIHostReady = 0x228;
// UnhideRequest of the trays' hover timers, and TrayUnhideFlags without the
// slide.
constexpr int kUnhideHover = 15;
constexpr int kUnhideInPlace = 1;
// GetAutoHideFlags: auto-hide on, and the taskbar hidden.
constexpr UINT kAutoHideOn = 1;
constexpr UINT kAutoHideHidden = 2;
// TrayUI's own hide check, as TrayUI::_SetAutoHideTimer sets it.
constexpr UINT_PTR kTrayHideTimer = 2;
constexpr UINT kTrayHideMs = 500;

using TrayUnhide_t = void(WINAPI*)(void* pThis, int flags, int request);
TrayUnhide_t TrayUI_Unhide_Original;
TrayUnhide_t CSecondaryTray__Unhide_Original;
using GetAutoHideFlags_t = UINT(WINAPI*)(void* pThis);
GetAutoHideFlags_t TrayUI_GetAutoHideFlags;
void* TrayUI_IInspectable_vftable;
void* TrayUI_ITrayComponentHost_vftable;
void* CSecondaryTray_CImpWndProc_vftable;
void** g_trayUI;  // Taskbar.dll's ComPtr<ITrayUI>
// The taskbar thread, where the trays run.
std::atomic<DWORD> g_trayThread;

// Counted with the size hooks, which Wh_ModUninit waits for: a tray's slide
// runs inside these calls.
struct InFlight {
    InFlight() { size::g_hookCalls++; }
    ~InFlight() { size::g_hookCalls--; }
};

// Committed memory that can be read, all of [p, p + n).
bool Readable(const void* p, size_t n) {
    constexpr DWORD kRead = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
                            PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                            PAGE_EXECUTE_WRITECOPY;
    MEMORY_BASIC_INFORMATION info;
    return p && VirtualQuery(p, &info, sizeof(info)) && info.State == MEM_COMMIT &&
           (info.Protect & kRead) && !(info.Protect & (PAGE_NOACCESS | PAGE_GUARD)) &&
           (const char*)p + n <= (const char*)info.BaseAddress + info.RegionSize;
}

bool OnTrayThread(HWND hwnd) {
    return GetWindowThreadProcessId(hwnd, nullptr) == GetCurrentThreadId();
}

void TrayCheckFailed(PCWSTR what, const void* at) {
    static bool logged;
    // A tray that can't be asked fails on every Unhide: noted once per change.
    static PCWSTR lastWhat;
    static const void* lastAt;
    if (what != lastWhat || at != lastAt) {
        lastWhat = what;
        lastAt = at;
        core::DiagLine(L"Tray check failed: %s %p", what, at);
    }
    if (!std::exchange(logged, true)) {
        Wh_Log(L"Auto-hide: the tray can't be asked (%s)", what);
    }
}

// The primary tray's ITrayUI, once it's TrayUI's and initialized; else null.
char* TrayUIInterface() {
    if (!TrayUI_Unhide_Original || !TrayUI_GetAutoHideFlags ||
        !TrayUI_IInspectable_vftable || !TrayUI_ITrayComponentHost_vftable) {
        return nullptr;
    }
    char* object = nullptr;
    if (g_trayUI && Readable(g_trayUI, sizeof(void*)) && *g_trayUI) {
        object = (char*)*g_trayUI - kTrayUIInterface;
    } else if (void* tray = g_tray) {
        object = (char*)tray;
    }
    if (!Readable(object, kTrayUIHost + sizeof(void*))) {
        TrayCheckFailed(L"object", object);
        return nullptr;
    }
    if (*(void**)object != TrayUI_IInspectable_vftable ||
        *(void**)(object + kTrayUIHost) != TrayUI_ITrayComponentHost_vftable) {
        TrayCheckFailed(L"vftables", object);
        return nullptr;
    }
    char* ui = object + kTrayUIInterface;
    void** slots = *(void***)ui;
    if (!Readable(slots + kGetAutoHideFlagsSlot, sizeof(void*)) ||
        slots[kGetAutoHideFlagsSlot] != (void*)TrayUI_GetAutoHideFlags) {
        TrayCheckFailed(L"slot", slots);
        return nullptr;
    }
    void** ready = (void**)(object + kTrayUIHost + kTrayUIHostReady);
    if (!Readable(ready, sizeof(void*)) || !Readable(*ready, sizeof(void*))) {
        TrayCheckFailed(L"not ready", ready);
        return nullptr;
    }
    return ui;
}

char* TrayUIHost(char* ui) {
    return ui - kTrayUIInterface + kTrayUIHost;
}

// A secondary taskbar's CSecondaryTray, as its window keeps it; else null.
char* SecondaryTrayOf(HWND hwnd) {
    if (!CSecondaryTray__Unhide_Original || !CSecondaryTray_CImpWndProc_vftable ||
        !core::IsSecondary(hwnd)) {
        return nullptr;
    }
    auto tray = (char*)GetWindowLongPtrW(hwnd, 0);
    if (!Readable(tray, 2 * sizeof(void*)) ||
        *(void**)tray != CSecondaryTray_CImpWndProc_vftable ||
        *(HWND*)(tray + sizeof(void*)) != hwnd) {
        TrayCheckFailed(L"secondary", tray);
        return nullptr;
    }
    return tray;
}

// Brings a hidden taskbar out as its tray's hover timer does for the pointer
// on its edge; false if the tray can't be asked. Its slide runs inside.
bool Reveal(HWND hwnd) {
    if (!Active() || !OnTrayThread(hwnd) || !Collapsed(hwnd)) {
        return false;
    }
    InFlight counted;
    if (core::IsSecondary(hwnd)) {
        char* tray = SecondaryTrayOf(hwnd);
        if (!tray) {
            return false;
        }
        CSecondaryTray__Unhide_Original(tray, 0, kUnhideHover);
        return true;
    }
    char* ui = TrayUIInterface();
    if (!ui) {
        return false;
    }
    TrayUI_Unhide_Original(TrayUIHost(ui), 0, kUnhideHover);
    return true;
}

bool LooksHidden(HWND taskbar);

// Explorer brings a hidden taskbar out as the mod's changes land, and nothing
// starts the primary tray's hide check then; the secondaries start their own.
void ResumeAutoHide(HWND hwnd) {
    char* ui = OnTrayThread(hwnd) ? TrayUIInterface() : nullptr;
    if (!ui) {
        core::DiagLine(L"Resume %p unverified", hwnd);
        return;
    }
    UINT flags = TrayUI_GetAutoHideFlags(ui);
    PCWSTR done = L"none";
    if ((flags & (kAutoHideOn | kAutoHideHidden)) == (kAutoHideOn | kAutoHideHidden)) {
        // Marked hidden while it's out: this marks it out and starts the check.
        TrayUI_Unhide_Original(TrayUIHost(ui), kUnhideInPlace, 0);
        done = L"unhide";
    } else if (flags & kAutoHideOn) {
        done = SetTimer(hwnd, kTrayHideTimer, kTrayHideMs, nullptr) ? L"timer" : L"no timer";
    }
    core::DiagLine(L"Resume %p flags %u -> %u %s", hwnd, flags, TrayUI_GetAutoHideFlags(ui),
                   done);
}

void SendBackIn() {
    for (HWND hwnd : std::exchange(g_outAtApply, {})) {
        // As it is now: a relayout since may have hidden it.
        if (IsWindow(hwnd) && !core::IsSecondary(hwnd) && !LooksHidden(hwnd)) {
            ResumeAutoHide(hwnd);
        }
    }
}

// ---- Bloom's launcher -----------------------------------------------------------------
// As with the Start menu, the hidden taskbar of the launcher's monitor comes
// out while Bloom's launcher is in front: it is revealed as the edge would,
// and its tray's hide check (a repeating timer) skips its ticks until the
// launcher leaves the front. The island sees it as any other reveal.

// Taskbar thread only.
HWINEVENTHOOK g_foregroundHook;
DWORD g_hookThread;           // the thread that set the hook
HWINEVENTHOOK g_leftoverHook;  // a hook UnhookWinEvent refused
HWND g_heldOut;   // the taskbar kept out
HWND g_launcher;  // the launcher window it is kept out for
bool g_reRevealQueued;
int g_reReveals;  // times the held taskbar was brought out again this hold
// A taskbar that keeps going in while held (a hide check whose ticks aren't
// the ones skipped) is brought out this many times, not over and over.
constexpr int kMaxReReveals = 2;

bool LauncherInFront() {
    return g_launcher && GetForegroundWindow() == g_launcher && IsWindowVisible(g_launcher);
}

// The taskbar of the monitor, else the primary.
HWND TaskbarOnMonitor(HMONITOR monitor) {
    HWND primary = nullptr;
    for (HWND hwnd : core::Taskbars()) {
        if (core::MonitorOf(hwnd) == monitor) {
            return hwnd;
        }
        if (!primary && !core::IsSecondary(hwnd)) {
            primary = hwnd;
        }
    }
    return primary;
}

HWND TaskbarFor(HWND window) {
    return TaskbarOnMonitor(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY));
}

// The hide check runs on from where it is; the primary's is started again in
// case something stopped it while the taskbar was held.
void ReleaseHeld() {
    HWND hwnd = std::exchange(g_heldOut, nullptr);
    g_launcher = nullptr;
    g_reReveals = 0;
    if (!hwnd) {
        return;
    }
    core::DiagLine(L"Bloom release %p", hwnd);
    if (IsWindow(hwnd) && !Collapsed(hwnd)) {
        if (!core::IsSecondary(hwnd)) {
            ResumeAutoHide(hwnd);
        }
        // The launcher is gone: the hide check (a secondary's under the same
        // id, see DropsTimer) looks now rather than a tick later, so the
        // taskbar goes in (and the island back) as it closes.
        PostMessage(hwnd, WM_TIMER, kTrayHideTimer, 0);
    }
}

// Holds the taskbar of the launcher's monitor while the launcher is in front,
// else lets it go. Only a taskbar Windows auto-hides is held. The launcher
// check opens Bloom's process only for a window titled "Bloom".
void EvaluateForeground() {
    if (!Active() || !g_foregroundHook) {
        return;
    }
    APPBARDATA data{.cbSize = sizeof(data)};
    if (!(SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) ||
        settings::g.autohide.keepHiddenForBloom || !start::LauncherAhead()) {
        ReleaseHeld();
        return;
    }
    HWND front = GetForegroundWindow();
    HWND taskbar = TaskbarFor(front);
    if (taskbar != g_heldOut || front != g_launcher) {
        ReleaseHeld();
    }
    g_heldOut = taskbar;
    g_launcher = front;
    bool asked = taskbar && Collapsed(taskbar) && Reveal(taskbar);
    core::DiagLine(L"Bloom hold %p reveal %d", taskbar, asked);
}

// ---- the notification center ------------------------------------------------------
// The notification center (Win+N, the clock) is the front window while it is open.
// Its taskbar and it then act as one, as a menu does: the taskbar stays out while
// the pointer is on either or between them, comes out when the pointer comes onto
// the panel, and goes in after the panel closes. The panel closes first: as the
// taskbar goes, explorer stretches the panel's window over the taskbar's place.

// Taskbar thread only.
HWND g_center;         // the notification center's window while it is in front
HWND g_centerTaskbar;  // the taskbar of its monitor
UINT_PTR g_centerTimer;
ULONGLONG g_centerClosing;  // when the mod asked the panel to close, 0 if it hasn't
HWND g_lastApp;  // the last front window other than a taskbar or the panel
constexpr UINT kCenterLookMs = 150;
// A panel that hasn't closed by then lets the taskbar go in anyway.
constexpr ULONGLONG kCenterCloseMs = 1000;

bool CenterInFront() {
    return g_center && GetForegroundWindow() == g_center;
}

// The panel on screen, widened to the taskbar's edge of the monitor so the way
// from the taskbar to the panel counts too. The window is the work area's height
// and shows only the panel, through its region.
bool CenterArea(RECT& area) {
    RECT box, bar;
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetWindowRect(g_center, &area) || !GetWindowRect(g_centerTaskbar, &bar) ||
        !GetMonitorInfo(core::MonitorOf(g_centerTaskbar), &mi)) {
        return false;
    }
    if (GetWindowRgnBox(g_center, &box) > NULLREGION) {
        OffsetRect(&box, area.left, area.top);
        area = box;
    }
    if (bar.top + bar.bottom < mi.rcMonitor.top + mi.rcMonitor.bottom) {
        area.top = mi.rcMonitor.top;
    } else {
        area.bottom = mi.rcMonitor.bottom;
    }
    return true;
}

bool PointerOnCenter() {
    RECT area;
    POINT pt;
    return CenterInFront() && GetCursorPos(&pt) && CenterArea(area) && PtInRect(&area, pt);
}

// Closes the panel as Esc closes it; it is in front, so the key goes to it. False
// while a modifier is held, as Ctrl+Esc opens Start.
bool CloseCenter() {
    if (!CenterInFront()) {
        return false;
    }
    for (int vk : {VK_CONTROL, VK_MENU, VK_SHIFT, VK_LWIN, VK_RWIN}) {
        if (GetAsyncKeyState(vk) & 0x8000) {
            return false;
        }
    }
    INPUT keys[2]{};
    keys[0].type = keys[1].type = INPUT_KEYBOARD;
    keys[0].ki.wVk = keys[1].ki.wVk = VK_ESCAPE;
    keys[1].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT sent = SendInput(2, keys, sizeof(INPUT));
    core::DiagLine(L"Notification center closes for its taskbar %p: %u", g_centerTaskbar, sent);
    return sent == 2;
}

void EndCenter() {
    if (g_centerTimer) {
        KillTimer(nullptr, std::exchange(g_centerTimer, 0));
    }
    HWND taskbar = std::exchange(g_centerTaskbar, nullptr);
    if (std::exchange(g_center, nullptr)) {
        core::DiagLine(L"Notification center lets go %p", taskbar);
    }
    // Closed for the taskbar to go in. A panel opened from the clock gives the front
    // back to the taskbar, which Windows keeps out while it is in front: the front
    // goes to the app that had it before, else the desktop. The check then looks now
    // rather than a tick later.
    if (std::exchange(g_centerClosing, 0) && taskbar && IsWindow(taskbar) && !Collapsed(taskbar)) {
        if (core::IsTaskbarWindow(GetForegroundWindow())) {
            HWND app = g_lastApp && IsWindowVisible(g_lastApp) && !IsIconic(g_lastApp)
                           ? g_lastApp
                           : GetShellWindow();
            BOOL moved = app && SetForegroundWindow(app);
            core::DiagLine(L"Notification center closed: front to %p %d", app, moved);
        }
        PostMessage(taskbar, WM_TIMER, kTrayHideTimer, 0);
    }
}

// While the panel is in front: the pointer coming onto it brings a hidden taskbar out.
void CALLBACK CenterTick(HWND, UINT, UINT_PTR, DWORD) {
    core::Guard(L"Notification center", [] {
        if (!Active() || !CenterInFront()) {
            return EndCenter();
        }
        if (Collapsed(g_centerTaskbar) && PointerOnCenter()) {
            Reveal(g_centerTaskbar);
        }
    });
}

// On a foreground change. Only a taskbar Windows auto-hides is held.
void EvaluateCenter() {
    HWND front = GetForegroundWindow();
    if (front && !core::IsTaskbarWindow(front) && !panels::IsNotificationCenter(front)) {
        g_lastApp = front;
    }
    APPBARDATA data{.cbSize = sizeof(data)};
    if (!Active() || !front || !(SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) ||
        !panels::IsNotificationCenter(front)) {
        return EndCenter();
    }
    if (front == g_center) {
        return;
    }
    EndCenter();
    RECT panel;
    if (!GetWindowRect(front, &panel)) {
        return;
    }
    RECT box;
    if (GetWindowRgnBox(front, &box) > NULLREGION) {
        OffsetRect(&box, panel.left, panel.top);
        panel = box;
    }
    g_center = front;
    g_centerTaskbar = TaskbarOnMonitor(MonitorFromRect(&panel, MONITOR_DEFAULTTOPRIMARY));
    g_centerTimer = g_centerTaskbar ? SetTimer(nullptr, 0, kCenterLookMs, CenterTick) : 0;
    core::DiagLine(L"Notification center holds %p", g_centerTaskbar);
}

// The held taskbar's hide check skips its ticks while the pointer is on the panel
// or on the way to it; on the taskbar the check keeps it out itself. A tick that
// would take the taskbar in closes the panel instead, and the taskbar follows once
// it has (EndCenter).
bool CenterHolds(HWND hwnd) {
    if (!g_center || hwnd != g_centerTaskbar || !Active() || !CenterInFront()) {
        return false;
    }
    POINT pt;
    RECT bar;
    if (PointerOnCenter() || !GetCursorPos(&pt)) {
        return true;
    }
    if (GetWindowRect(hwnd, &bar) && PtInRect(&bar, pt)) {
        return false;
    }
    if (g_centerClosing) {
        return GetTickCount64() - g_centerClosing < kCenterCloseMs;
    }
    if (!CloseCenter()) {
        return false;
    }
    g_centerClosing = GetTickCount64();
    return true;
}

// The taskbar went in another way with the panel in front: the panel closes too.
void CloseCenterWith(HWND hwnd) {
    if (g_center && hwnd == g_centerTaskbar) {
        core::Post([] { CloseCenter(); });
    }
}

// Once per foreground change. The callback can come while explorer waits in
// its own SendMessage or a tray slide, so the work runs later from the queue.
// The launcher's first opening in a Bloom run is named in front a moment before
// Windows makes it the front window, and no event follows when it does: it is
// looked at again every kLauncherLookMs, kLauncherLooks times at most.
constexpr UINT kLauncherLookMs = 50;
constexpr int kLauncherLooks = 10;
UINT_PTR g_launcherTimer;
HWND g_launcherNamed;  // the launcher named in front while it wasn't yet
int g_launcherLooks;

void EndLauncherLook() {
    if (g_launcherTimer) {
        KillTimer(nullptr, std::exchange(g_launcherTimer, 0));
    }
    g_launcherNamed = nullptr;
}

void CALLBACK LauncherLook(HWND, UINT, UINT_PTR, DWORD) {
    core::Guard(L"Bloom launcher", [] {
        HWND named = g_launcherNamed;
        bool front = GetForegroundWindow() == named;
        if (front || --g_launcherLooks <= 0 || !IsWindowVisible(named)) {
            EndLauncherLook();
            core::DiagLine(L"Bloom launcher %p in front later: %d", named, front);
            if (front) {
                EvaluateForeground();
            }
        }
    });
}

void CALLBACK OnForeground(HWINEVENTHOOK, DWORD, HWND window, LONG, LONG, DWORD, DWORD) {
    core::Guard(L"Bloom launcher", [window] {
        core::Post([window] {
            EvaluateForeground();
            EvaluateCenter();
            if (window == GetForegroundWindow()) {
                return;
            }
            EndLauncherLook();
            if (Active() && start::IsLauncher(window)) {
                g_launcherNamed = window;
                g_launcherLooks = kLauncherLooks;
                g_launcherTimer = SetTimer(nullptr, 0, kLauncherLookMs, LauncherLook);
            }
        });
    });
}

// A held taskbar that went in by another way than the hide check whose ticks
// are skipped comes out again while the launcher is in front, once per time it
// goes in and at most kMaxReReveals times per hold.
void KeepHeldOut(HWND hwnd) {
    if (!g_heldOut || hwnd != g_heldOut || g_reRevealQueued || !LauncherInFront()) {
        return;
    }
    if (g_reReveals >= kMaxReReveals) {
        core::DiagLine(L"Bloom reveal again %p skipped after %d", hwnd, g_reReveals);
        return;
    }
    g_reRevealQueued = core::Post([hwnd] {
        g_reRevealQueued = false;
        if (hwnd == g_heldOut && LauncherInFront()) {
            g_reReveals++;
            bool asked = Reveal(hwnd);
            core::DiagLine(L"Bloom reveal again %p %d", hwnd, asked);
        }
    });
}

// The held taskbar's hide check is skipped while the launcher is still the
// visible front window; a launcher gone without a foreground event lets the
// tick through. A secondary tray's check is taken to use TrayUI's timer id;
// were it another, KeepHeldOut brings it out a bounded number of times.
bool DropsTimer(HWND hwnd, UINT_PTR id) {
    if (id == kTrayHideTimer && CenterHolds(hwnd)) {
        return true;
    }
    if (id != kTrayHideTimer || hwnd != g_heldOut || !g_heldOut) {
        return false;
    }
    if (Active() && LauncherInFront()) {
        return true;
    }
    ReleaseHeld();
    return false;
}

void WatchLauncher() {
    // Set whatever keepHiddenForBloom says: the notification center uses it too.
    if (g_foregroundHook) {
        return;
    }
    // Out of context: called on this thread, which unhooks it in Restore.
    g_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                       nullptr, OnForeground, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_foregroundHook) {
        Wh_Log(L"Auto-hide: foreground hook failed");
        return;
    }
    g_hookThread = GetCurrentThreadId();
    core::g_dropsTimer = DropsTimer;
    // A launcher already in front: looked at once, after the work the apply
    // posts (its SendBackIn), as the posted work runs in order.
    core::Post([] { core::Post(EvaluateForeground); });
}

void UnwatchLauncher() {
    core::g_dropsTimer = nullptr;
    EndCenter();
    EndLauncherLook();
    if (HWINEVENTHOOK hook = std::exchange(g_foregroundHook, nullptr)) {
        if (!UnhookWinEvent(hook)) {
            g_leftoverHook = hook;
            Wh_Log(L"Auto-hide: foreground unhook failed (%lu)", GetLastError());
        }
    }
    if (core::g_unloading) {
        // The tray's own check, never stopped, takes it in.
        g_heldOut = g_launcher = nullptr;
    } else {
        ReleaseHeld();
    }
}

// Wh_ModUninit, after the taskbar thread's work: a hook still set (its unhook
// failed or never ran there) is tried once more, as the callback's code goes.
void UnhookLeftover() {
    for (HWINEVENTHOOK* slot : {&g_foregroundHook, &g_leftoverHook}) {
        if (HWINEVENTHOOK hook = std::exchange(*slot, nullptr)) {
            BOOL done = UnhookWinEvent(hook);
            DWORD error = done ? 0 : GetLastError();
            Wh_Log(L"Auto-hide: foreground unhook at unload %d (%lu), hook thread %lu, here %lu",
                   done, error, g_hookThread, GetCurrentThreadId());
        }
    }
}

// The taskbar a tray call is for (1 the primary's TrayUI, 2 a CSecondaryTray);
// null if it isn't known yet. g_collapsed keeps the windows of taskbars that
// are gone (a monitor went), which no longer pass as taskbars.
HWND TrayTaskbar(int tray, void* pThis) {
    for (auto& [h, collapsed] : g_collapsed) {
        if (core::IsTaskbarWindow(h) &&
            (tray == 1 ? !core::IsSecondary(h) : SecondaryTrayOf(h) == pThis)) {
            return h;
        }
    }
    return nullptr;
}

// A tray's hover timer asking to bring a hidden taskbar out that the island
// keeps in. A held button is a drag, which still brings it out to drop on.
// Only while the island can ask the tray itself (a secondary tray found is
// one it can ask), so the taskbar can't end up with no way out.
bool HeldHover(int tray, void* pThis, int request) {
    if (request == kUnhideHover && PointerOnHolder()) {
        return true;  // a window that asks to hold is under the pointer
    }
    if (request != kUnhideHover || !g_holdsHover ||
        ((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON)) & 0x8000)) {
        return false;
    }
    HWND hwnd = TrayTaskbar(tray, pThis);
    return hwnd && Collapsed(hwnd) && (tray == 2 || TrayUIInterface()) &&
           g_holdsHover(hwnd);
}

// UnhideRequest of an app flashing its taskbar button. Explorer asks again
// while the button flashes and keeps the taskbar out until the app is
// activated; dropping every one of them leaves auto-hide as it was.
constexpr int kUnhideFlash = 17;

// Diagnostics: who unhides a taskbar, with the pointer where.
void LogUnhide(int tray, void* pThis, int flags, int request, void* caller) {
    HWND hwnd = TrayTaskbar(tray, pThis);
    POINT pt{}, physical{};
    GetCursorPos(&pt);
    GetPhysicalCursorPos(&physical);
    DWORD pos = GetMessagePos();
    RECT rc{};
    if (hwnd) {
        GetWindowRect(hwnd, &rc);
    }
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (PCWSTR)caller, &module);
    core::DiagLine(L"Unhide %d %p flags %d request %d at %d,%d physical %d,%d msgpos %d,%d "
                   L"rect %d,%d-%d,%d collapsed %d caller +%llx",
                   tray, hwnd, flags, request, pt.x, pt.y, physical.x, physical.y,
                   (short)LOWORD(pos), (short)HIWORD(pos), rc.left, rc.top, rc.right,
                   rc.bottom, hwnd && Collapsed(hwnd),
                   (unsigned long long)((char*)caller - (char*)module));
    if (tray == 2 && request == kUnhideHover) {
        auto const& hit = core::g_lastHitTest;
        core::DiagLine(L"  last hit test %p %lu ms ago at %d,%d cursor %d,%d send %x", hit.hwnd,
                       GetTickCount() - hit.tick, hit.at.x, hit.at.y, hit.cursor.x,
                       hit.cursor.y, hit.send);
    }
}

// True if the call is dropped. The island's own requests call the originals
// and don't come through here.
bool OnUnhide(int tray, void* pThis, int flags, int request, void* caller) {
    // g_collapsed is the taskbar thread's.
    if (!Active() || GetCurrentThreadId() != g_trayThread) {
        return false;
    }
    core::Guard(L"Unhide diag", [&] { LogUnhide(tray, pThis, flags, request, caller); });
    if (request == kUnhideFlash && settings::g.autohide.keepHiddenOnFlash) {
        return true;
    }
    bool held = false;
    core::Guard(L"Unhide hold", [&] { held = HeldHover(tray, pThis, request); });
    return held;
}

void WINAPI TrayUI_Unhide_Hook(void* pThis, int flags, int request) {
    InFlight counted;
    if (!OnUnhide(1, pThis, flags, request, __builtin_return_address(0))) {
        TrayUI_Unhide_Original(pThis, flags, request);
    }
}

void WINAPI CSecondaryTray__Unhide_Hook(void* pThis, int flags, int request) {
    InFlight counted;
    if (!OnUnhide(2, pThis, flags, request, __builtin_return_address(0))) {
        CSecondaryTray__Unhide_Original(pThis, flags, request);
    }
}

// ---- Taskbar.View.dll: slide animations -------------------------------------------
// The taskbar reuses one animation object per direction; its duration is set
// when it's first seen and on each apply.

struct Animation {
    wuc::KeyFrameAnimation animation{nullptr};
    wf::TimeSpan original{};
};

[[clang::no_destroy]] Animation g_expand;
[[clang::no_destroy]] Animation g_collapse;

void SetDuration(Animation& a, bool stock) {
    if (!a.animation) {
        return;
    }
    int ms = settings::g.autohide.animation;
    // With the island the slide keeps time with its morph.
    if (ms == 0 && settings::g.island.enabled) {
        ms = settings::theme::kIslandMorphMs;
    }
    // The taskbar's content is hidden while the island grows; it is in place
    // at once, for the island to fade onto.
    if (&a == &g_expand && g_expandUnderIsland) {
        ms = 1;
    }
    wf::TimeSpan duration = a.original;
    if (!stock && ms > 0) {
        duration = std::chrono::milliseconds(ms);
    }
    core::DiagLine(L"Duration %s %p %lld ms", &a == &g_expand ? L"expand" : L"collapse",
                   winrt::get_abi(a.animation), (long long)(duration.count() / 10000));
    core::Guard(L"Animation duration", [&] { a.animation.Duration(duration); });
}

// `result`: the returned std::pair<KeyFrameAnimation, telemetry>.
void NoteAnimation(Animation& a, void** result) {
    wuc::KeyFrameAnimation animation{nullptr};
    if (result) {
        winrt::copy_from_abi(animation, result[0]);
    }
    if (!animation || animation == a.animation) {
        return;
    }
    a.animation = animation;
    a.original = animation.Duration();
    core::DiagLine(L"Animation %s new %p", &a == &g_expand ? L"expand" : L"collapse",
                   winrt::get_abi(animation));
    SetDuration(a, !Active());
}

using SharedAnimation_t = void**(WINAPI*)(void* pThis, void* result);
SharedAnimation_t SharedAnimations_TaskbarExpandAnimation_Original;
void** WINAPI SharedAnimations_TaskbarExpandAnimation_Hook(void* pThis,
                                                           void* result) {
    void** ret = SharedAnimations_TaskbarExpandAnimation_Original(pThis, result);
    core::Guard(L"Expand animation", [&] { NoteAnimation(g_expand, ret); });
    return ret;
}

SharedAnimation_t SharedAnimations_TaskbarCollapseAnimation_Original;
void** WINAPI SharedAnimations_TaskbarCollapseAnimation_Hook(void* pThis,
                                                             void* result) {
    void** ret =
        SharedAnimations_TaskbarCollapseAnimation_Original(pThis, result);
    core::Guard(L"Collapse animation", [&] { NoteAnimation(g_collapse, ret); });
    return ret;
}

// ---- registration and lifecycle -----------------------------------------------------

void CollectTaskbarDll(std::vector<SH>& list) {
    list.push_back({{LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_61970341>::__private_IsEnabled(void))"},
                    &WilFeature_61970341_IsEnabled_Original,
                    WilFeature_61970341_IsEnabled_Hook, true});
    // The trays; each one missing turns off only what needs it.
    list.push_back({{LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
                    &TrayUI_Unhide_Original, TrayUI_Unhide_Hook, true});
    list.push_back({{LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
                    &CSecondaryTray__Unhide_Original, CSecondaryTray__Unhide_Hook, true});
    list.push_back({{LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"},
                    &TrayUI_GetAutoHideFlags, nullptr, true});
    list.push_back({{LR"(const TrayUI::`vftable'{for `IInspectable'})"},
                    &TrayUI_IInspectable_vftable, nullptr, true});
    list.push_back({{LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
                    &TrayUI_ITrayComponentHost_vftable, nullptr, true});
    list.push_back({{LR"(const CSecondaryTray::`vftable'{for `CImpWndProc'})"},
                    &CSecondaryTray_CImpWndProc_vftable, nullptr, true});
    list.push_back({{LR"(class Microsoft::WRL::ComPtr<struct ITrayUI> g_trayUI)"},
                    &g_trayUI, nullptr, true});
}

void CollectTaskbarView(std::vector<SH>& list) {
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
                    &ViewCoordinator_UpdateIsExpanded_Original,
                    ViewCoordinator_UpdateIsExpanded_Hook, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpandedAfterHoverDelay(unsigned __int64,bool,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
                    &ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Original,
                    ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Hook, true});
    list.push_back({{LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::IsExpanded(unsigned __int64))"},
                    &ViewCoordinator_IsExpanded_Original, nullptr, true});
    list.push_back({{LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
                    &ViewCoordinator_ShouldTaskbarBeExpanded_Original, nullptr, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsExpandedChanged(unsigned __int64))"},
                    &ViewCoordinator_HandleIsExpandedChanged_Original,
                    ViewCoordinator_HandleIsExpandedChanged_Hook, true});
    list.push_back({{LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarExpandAnimation(void)const )"},
                    &SharedAnimations_TaskbarExpandAnimation_Original,
                    SharedAnimations_TaskbarExpandAnimation_Hook, true});
    list.push_back({{LR"(public: struct std::pair<struct winrt::Windows::UI::Composition::KeyFrameAnimation,struct winrt::WindowsUdk::UI::Composition::AnimationLastFrameTelemetry> __cdecl winrt::Taskbar::implementation::SharedAnimations::TaskbarCollapseAnimation(void)const )"},
                    &SharedAnimations_TaskbarCollapseAnimation_Original,
                    SharedAnimations_TaskbarCollapseAnimation_Hook, true});
}

// A taskbar whose state isn't known yet counts as hidden when auto-hide is on
// and its window is off the monitor (Windows slides the window itself).
bool LooksHidden(HWND taskbar) {
    APPBARDATA data{.cbSize = sizeof(data)};
    if (!(SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE)) {
        return false;
    }
    RECT rc;
    MONITORINFO monitor{.cbSize = sizeof(monitor)};
    return GetWindowRect(taskbar, &rc) &&
           GetMonitorInfo(core::MonitorOf(taskbar), &monitor) &&
           (rc.top < monitor.rcMonitor.top || rc.bottom > monitor.rcMonitor.bottom);
}

void Apply(HWND taskbar) {
    if (core::g_vertical) {
        return;
    }
    static bool logged;
    if (!logged) {
        logged = true;
        Wh_Log(L"Auto-hide: top %d, requests %d, hover %d, animations %d/%d",
               WilFeature_61970341_IsEnabled_Original != nullptr,
               ViewCoordinator_UpdateIsExpanded_Original &&
                   ViewCoordinator_IsExpanded_Original &&
                   ViewCoordinator_ShouldTaskbarBeExpanded_Original,
               ViewCoordinator_UpdateIsExpandedAfterHoverDelay_Original != nullptr,
               SharedAnimations_TaskbarExpandAnimation_Original != nullptr,
               SharedAnimations_TaskbarCollapseAnimation_Original != nullptr);
    }
    g_trayThread = GetWindowThreadProcessId(taskbar, nullptr);
    static bool checked;
    if (!std::exchange(checked, true)) {
        char* ui = TrayUIInterface();
        core::DiagLine(L"Tray symbols: Unhide %d _Unhide %d flags %d vftables %d/%d/%d "
                       L"trayUI %d tray %d; primary %p",
                       TrayUI_Unhide_Original != nullptr,
                       CSecondaryTray__Unhide_Original != nullptr,
                       TrayUI_GetAutoHideFlags != nullptr,
                       TrayUI_IInspectable_vftable != nullptr,
                       TrayUI_ITrayComponentHost_vftable != nullptr,
                       CSecondaryTray_CImpWndProc_vftable != nullptr, g_trayUI != nullptr,
                       g_tray.load() != nullptr, ui);
    }
    if (core::IsSecondary(taskbar)) {
        core::DiagLine(L"Secondary tray %p: %p", taskbar, SecondaryTrayOf(taskbar));
    }
    core::g_onTaskbarMoved = NoteWindowPos;
    WatchLauncher();
    NoteWindowPos(taskbar);
    SetDuration(g_expand, false);
    SetDuration(g_collapse, false);
    if (!LooksHidden(taskbar)) {
        g_outAtApply.push_back(taskbar);
        core::DiagLine(L"Out at apply %p", taskbar);
        // Once the apply's calls are done; it looks at the window again then.
        core::Post(SendBackIn);
    }
}

void Restore() {
    core::g_onTaskbarMoved = nullptr;
    UnwatchLauncher();
    for (auto& [hwnd, pending] : g_pending) {
        KillTimer(hwnd, kTimerId);
    }
    g_pending.clear();
    g_outAtApply.clear();
    g_expandUnderIsland = false;
    SetDuration(g_expand, true);
    SetDuration(g_collapse, true);
    if (core::g_unloading) {
        g_collapsed.clear();
        g_expand = {};
        g_collapse = {};
    }
}

}  // namespace autohide

// ============================================================================
namespace media {
// ============================================================================
// F6: the current media session's art, title and transport buttons. One
// worker thread owns every GSMTC object; the event handlers only flag what
// changed and wake it, so bursts collapse into one refresh, and the taskbar
// thread only gets the finished snapshot.

namespace theme = settings::theme;
namespace wmc = winrt::Windows::Media::Control;
namespace wss = winrt::Windows::Storage::Streams;
using winrt::box_value;

// What the worker should look at again.
constexpr uint32_t kSession = 1, kProps = 2, kPlayback = 4, kResend = 8;
// Button commands.
constexpr uint32_t kPrevious = 1, kToggle = 2, kNext = 4;
constexpr uint64_t kMaxArtBytes = 8 << 20;

// ---- worker -------------------------------------------------------------------

HANDLE g_thread;
HANDLE g_wake;  // auto-reset
std::atomic<bool> g_stop;
std::atomic<uint32_t> g_dirty;
std::atomic<uint32_t> g_commands;
std::atomic<int> g_signalling;  // Signal calls in progress, for Stop
std::atomic<int> g_handlers;    // event handlers the sources still hold, for Stop
std::mutex g_opLock;            // g_op
[[clang::no_destroy]] wf::IAsyncInfo g_op{nullptr};  // what the worker waits on

// From event handlers and buttons, any thread.
void Signal(uint32_t dirty, uint32_t commands) {
    g_signalling++;
    if (!g_stop && g_wake) {
        g_dirty |= dirty;
        g_commands |= commands;
        SetEvent(g_wake);
    }
    g_signalling--;
}

// Blocks the worker on `op`; Stop cancels it, so the worker can't outlive the
// mod waiting on a slow player.
template <class Op>
auto Wait(Op const& op) {
    // A session on its way out can hand back no operation.
    if (!op) {
        winrt::throw_hresult(E_POINTER);
    }
    {
        std::lock_guard guard(g_opLock);
        g_op = op;
    }
    struct Clear {
        ~Clear() {
            std::lock_guard guard(g_opLock);
            g_op = nullptr;
        }
    } clear;
    if (g_stop) {
        op.Cancel();
    }
    return op.get();
}

struct Snapshot {
    bool visible = false;
    winrt::hstring title, artist;
    bool playing = false;
    bool canPrevious = false, canToggle = false, canNext = false;
    bool artChanged = false;
    wss::IRandomAccessStream art{nullptr};  // null with artChanged: no art
    wss::IBuffer artBytes{nullptr};         // the same art, for the island
};

void Show(Snapshot const& s);

// Captured by every event handler, so Stop sees when the sources have let go
// of them all and none can still be called.
struct HandlerRef {
    HandlerRef() { g_handlers++; }
    HandlerRef(HandlerRef const&) { g_handlers++; }
    ~HandlerRef() { g_handlers--; }
};

// The island (F8) mirrors what's shown; set between its Apply and Restore.
void (*g_onShow)(Snapshot const&) = nullptr;

// Worker thread only. Tokens are kept by hand: auto_revoke needs weak
// references, which the GSMTC proxies may not support.
struct Watcher {
    wmc::GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    winrt::event_token sessionsChanged{}, currentChanged{};
    wmc::GlobalSystemMediaTransportControlsSession session{nullptr};
    winrt::event_token propsChanged{}, playbackChanged{};
    winrt::hstring title, artist;
    uint64_t artHash = 0;  // of the art the widget shows, 0 for none
    bool shown = false;

    // Also on the way out of an exception: no subscription may outlive the
    // worker.
    ~Watcher() {
        Follow(nullptr);
        if (!manager) {
            return;
        }
        if (sessionsChanged.value) {
            core::Guard(L"Media sessions unsubscribe",
                        [&] { manager.SessionsChanged(sessionsChanged); });
        }
        if (currentChanged.value) {
            core::Guard(L"Media session unsubscribe",
                        [&] { manager.CurrentSessionChanged(currentChanged); });
        }
    }

    void Subscribe() {
        auto onSessions = [ref = HandlerRef()](auto&&, auto&&) {
            core::Guard(L"Media sessions changed", [] { Signal(kSession, 0); });
        };
        sessionsChanged = manager.SessionsChanged(onSessions);
        currentChanged = manager.CurrentSessionChanged(onSessions);
    }

    void Follow(wmc::GlobalSystemMediaTransportControlsSession const& next) {
        if (session && propsChanged.value) {
            core::Guard(L"Media properties unsubscribe",
                        [&] { session.MediaPropertiesChanged(propsChanged); });
        }
        if (session && playbackChanged.value) {
            core::Guard(L"Media playback unsubscribe",
                        [&] { session.PlaybackInfoChanged(playbackChanged); });
        }
        session = next;
        propsChanged = {};
        playbackChanged = {};
        if (!session) {
            return;
        }
        propsChanged = session.MediaPropertiesChanged([ref = HandlerRef()](auto&&, auto&&) {
            core::Guard(L"Media properties changed", [] { Signal(kProps, 0); });
        });
        playbackChanged = session.PlaybackInfoChanged([ref = HandlerRef()](auto&&, auto&&) {
            core::Guard(L"Media playback changed", [] { Signal(kPlayback, 0); });
        });
    }
};

uint64_t Hash(const uint8_t* p, uint32_t n) {
    uint64_t h = 14695981039346656037ull;  // FNV-1a
    for (uint32_t i = 0; i < n; i++) {
        h = (h ^ p[i]) * 1099511628211ull;
    }
    return h ? h : 1;
}

// Puts the art into `s` when it differs from what's shown. The bytes are
// compared, not the track: players often send the new title with the old art
// and the new art a moment later under the same title.
void ReadArt(Watcher& w,
             wss::IRandomAccessStreamReference const& thumbnail,
             Snapshot& s) {
    uint64_t hash = 0;
    wss::IRandomAccessStream copy{nullptr};
    wss::IBuffer raw{nullptr};
    if (thumbnail) {
        auto stream = Wait(thumbnail.OpenReadAsync());
        uint64_t size = stream ? stream.Size() : 0;
        if (size > 0 && size <= kMaxArtBytes) {
            wss::Buffer buffer((uint32_t)size);
            auto bytes = Wait(stream.ReadAsync(buffer, (uint32_t)size,
                                               wss::InputStreamOptions::None));
            if (bytes && bytes.Length() > 0) {
                hash = Hash(bytes.data(), bytes.Length());
            }
            if (hash && hash != w.artHash) {
                // An in-memory copy: the player's stream belongs to this
                // thread's apartment and the bitmap is made on the taskbar's.
                wss::InMemoryRandomAccessStream memory;
                Wait(memory.WriteAsync(bytes));
                memory.Seek(0);
                copy = memory;
                raw = bytes;
            }
        }
    }
    if (hash != w.artHash) {
        w.artHash = hash;
        s.artChanged = true;
        s.art = copy;
        s.artBytes = raw;
    }
}

void Refresh(Watcher& w, uint32_t dirty) {
    if (dirty & kResend) {
        w.artHash = 0;
        w.shown = false;
        dirty |= kSession | kProps | kPlayback;
    }
    if (dirty & kSession) {
        auto session = w.manager.GetCurrentSession();
        if (session != w.session) {
            w.Follow(session);
            dirty |= kProps | kPlayback;
        }
    }

    Snapshot s;
    // A session that is closing can return no playback info (S_OK and null);
    // it counts as nothing playing.
    auto info = w.session ? w.session.GetPlaybackInfo() : nullptr;
    if (info) {
        using Status = wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus;
        auto status = info.PlaybackStatus();
        s.visible = status == Status::Playing || status == Status::Paused ||
                    status == Status::Changing;
        s.playing = status == Status::Playing;
        if (auto controls = info.Controls()) {
            s.canPrevious = controls.IsPreviousEnabled();
            s.canToggle = controls.IsPlayPauseToggleEnabled() ||
                          (s.playing ? controls.IsPauseEnabled()
                                     : controls.IsPlayEnabled());
            s.canNext = controls.IsNextEnabled();
        }
    }

    if (!s.visible) {
        if (!w.shown && !(dirty & kResend)) {
            return;  // hidden already
        }
        w.shown = false;
        w.artHash = 0;
        w.title = w.artist = {};
        s.artChanged = true;  // drops the bitmap
    } else {
        if (!w.shown || (dirty & kProps)) {
            core::Guard(L"Media properties", [&] {
                auto props = Wait(w.session.TryGetMediaPropertiesAsync());
                if (!props) {
                    return;
                }
                w.title = props.Title();
                w.artist = props.Artist();
                core::Guard(L"Media art", [&] { ReadArt(w, props.Thumbnail(), s); });
            });
        }
        w.shown = true;
        s.title = w.title;
        s.artist = w.artist;
    }
    core::Post([s = std::move(s)] { Show(s); });
}

// Not awaited: the PlaybackInfoChanged that follows shows the result.
void Send(Watcher& w, uint32_t commands) {
    if (!w.session) {
        return;
    }
    if (commands & kPrevious) {
        w.session.TrySkipPreviousAsync();
    }
    if (commands & kToggle) {
        // Some players offer play and pause but not the toggle. Without
        // playback info (a closing session) the toggle is all there is.
        auto info = w.session.GetPlaybackInfo();
        auto controls = info ? info.Controls() : nullptr;
        if (!controls || controls.IsPlayPauseToggleEnabled()) {
            w.session.TryTogglePlayPauseAsync();
        } else if (info.PlaybackStatus() == wmc::
                       GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
            w.session.TryPauseAsync();
        } else {
            w.session.TryPlayAsync();
        }
    }
    if (commands & kNext) {
        w.session.TrySkipNextAsync();
    }
}

void Loop() {
    Watcher w;
    // Right after explorer starts the broker may not answer yet: asked again,
    // a while later each time, until it does or the mod stops.
    for (DWORD delay = 1000; !g_stop; delay = std::min<DWORD>(delay * 2, 60000)) {
        HRESULT hr = S_OK;
        try {
            w.manager = Wait(
                wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync());
        } catch (...) {
            hr = winrt::to_hresult();
        }
        if (w.manager) {
            break;
        }
        if (delay == 1000) {
            Wh_Log(L"No media session manager (%08X), asking again", hr);
        }
        WaitForSingleObject(g_wake, delay);
    }
    if (!w.manager) {
        return;
    }
    w.Subscribe();
    while (!g_stop) {
        uint32_t dirty = g_dirty.exchange(0);
        uint32_t commands = g_commands.exchange(0);
        if (!dirty && !commands) {
            WaitForSingleObject(g_wake, INFINITE);
            continue;
        }
        if (commands) {
            core::Guard(L"Media command", [&] { Send(w, commands); });
        }
        if (dirty) {
            core::Guard(L"Media refresh", [&] { Refresh(w, dirty); });
        }
    }
}

// `module`: the worker's own reference to this module, let go as it ends.
DWORD WINAPI Worker(void* module) {
    bool com = false;
    core::Guard(L"Media apartment", [&] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        com = true;
    });
    if (com) {
        core::Guard(L"Media worker", Loop);
        winrt::uninit_apartment();
    }
    if (module) {
        FreeLibraryAndExitThread((HMODULE)module, 0);
    }
    return 0;
}

// ---- widget (taskbar thread) ------------------------------------------------------

// One per taskbar: a XAML element lives in one tree.
struct Ui {
    HWND taskbar = nullptr;
    FrameworkElement root{nullptr};
    FrameworkElement info{nullptr};  // art and title, carries the tooltip
    Controls::Border art{nullptr};
    Controls::TextBlock title{nullptr};
    Controls::Button buttons[3]{nullptr, nullptr, nullptr};
    winrt::event_token clicks[3]{};
    std::wstring tip;
    int playing = -1;
};

[[clang::no_destroy]] std::list<Ui> g_uis;
std::vector<HWND> g_taskbars;  // applied, between Apply and Restore

// `themed`: theme brushes from the taskbar's resources; without them the
// stock control colors stay.
std::wstring WidgetXaml(bool themed) {
    namespace t = theme;
    auto n = [](int v) { return std::to_wstring(v); };
    auto brush = [&](PCWSTR attribute, PCWSTR key, PCWSTR plain) {
        std::wstring value = themed ? std::wstring(L"{ThemeResource ") + key + L"}"
                                    : std::wstring(plain ? plain : L"");
        return value.empty() ? std::wstring()
                             : std::wstring(L" ") + attribute + L"=\"" + value + L"\"";
    };
    constexpr PCWSTR kGlyphFont = L"Segoe Fluent Icons, Segoe MDL2 Assets";
    std::wstring radius = n(t::kMediaArtRadius);

    std::wstring xaml =
        L"<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\""
        L" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\""
        // Last in the pill (after the clock): no divider or padding of its
        // own, the pill's end padding follows.
        L" VerticalAlignment=\"Center\">"
        L"<Border.Resources>"
        L"<Style x:Key=\"PillMediaButton\" TargetType=\"Button\">"
        L"<Setter Property=\"Width\" Value=\"" + n(t::kMediaButtonWidth) + L"\"/>"
        L"<Setter Property=\"Height\" Value=\"" + n(t::kMediaButtonHeight) + L"\"/>"
        L"<Setter Property=\"Padding\" Value=\"0\"/>"
        L"<Setter Property=\"FontFamily\" Value=\"" + kGlyphFont + L"\"/>"
        L"<Setter Property=\"FontSize\" Value=\"" + n(t::kMediaGlyphSize) + L"\"/>" +
        (themed ? L"<Setter Property=\"Foreground\""
                  L" Value=\"{ThemeResource TextFillColorPrimaryBrush}\"/>"
                : L"") +
        L"<Setter Property=\"Template\"><Setter.Value>"
        L"<ControlTemplate TargetType=\"Button\">"
        L"<Grid Background=\"Transparent\">"
        L"<VisualStateManager.VisualStateGroups>"
        L"<VisualStateGroup x:Name=\"CommonStates\">"
        L"<VisualState x:Name=\"Normal\"/>"
        L"<VisualState x:Name=\"PointerOver\"><VisualState.Setters>"
        L"<Setter Target=\"Glyph.Foreground\" Value=\"" + t::kMediaHover + L"\"/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name=\"Pressed\"><VisualState.Setters>"
        L"<Setter Target=\"Glyph.Foreground\" Value=\"" + t::kMediaHover + L"\"/>"
        L"<Setter Target=\"Glyph.Opacity\" Value=\"0.7\"/>"
        L"</VisualState.Setters></VisualState>"
        L"<VisualState x:Name=\"Disabled\"><VisualState.Setters>"
        L"<Setter Target=\"Glyph.Opacity\" Value=\"0.35\"/>"
        L"</VisualState.Setters></VisualState>"
        L"</VisualStateGroup>"
        L"</VisualStateManager.VisualStateGroups>"
        L"<ContentPresenter x:Name=\"Glyph\" HorizontalAlignment=\"Center\""
        L" VerticalAlignment=\"Center\" Content=\"{TemplateBinding Content}\""
        L" FontFamily=\"{TemplateBinding FontFamily}\""
        L" FontSize=\"{TemplateBinding FontSize}\""
        L" Foreground=\"{TemplateBinding Foreground}\"/>"
        L"</Grid>"
        L"</ControlTemplate>"
        L"</Setter.Value></Setter>"
        L"</Style>"
        L"</Border.Resources>"
        L"<StackPanel Orientation=\"Horizontal\">"
        L"<StackPanel x:Name=\"Info\" Orientation=\"Horizontal\" Spacing=\"8\""
        L" Margin=\"0,0,4,0\" Background=\"Transparent\">"
        // The art covers the placeholder note; a failed decode leaves it showing.
        L"<Grid Width=\"" + n(t::kMediaArtSize) + L"\" Height=\"" + n(t::kMediaArtSize) +
        L"\" CornerRadius=\"" + radius + L"\"" +
        brush(L"Background", L"ControlFillColorSecondaryBrush", L"#26808080") +
        L">"
        L"<TextBlock Text=\"&#xE8D6;\" FontFamily=\"" + kGlyphFont +
        L"\" FontSize=\"14\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\"" +
        brush(L"Foreground", L"TextFillColorSecondaryBrush", nullptr) +
        L"/>"
        L"<Border x:Name=\"Art\" CornerRadius=\"" + radius + L"\"/>"
        L"</Grid>"
        L"<TextBlock x:Name=\"Title\" VerticalAlignment=\"Center\" FontSize=\"" +
        n(t::kMediaTitleSize) +
        L"\" TextWrapping=\"NoWrap\" TextTrimming=\"CharacterEllipsis\" MaxLines=\"1\"" +
        brush(L"Foreground", L"TextFillColorPrimaryBrush", nullptr) +
        L"/>"
        L"</StackPanel>"
        L"<Button x:Name=\"Previous\" Style=\"{StaticResource PillMediaButton}\""
        L" Content=\"&#xE892;\" AutomationProperties.Name=\"이전\"/>"
        L"<Button x:Name=\"Toggle\" Style=\"{StaticResource PillMediaButton}\""
        L" Content=\"&#xE768;\" AutomationProperties.Name=\"재생/일시정지\"/>"
        L"<Button x:Name=\"Next\" Style=\"{StaticResource PillMediaButton}\""
        L" Content=\"&#xE893;\" AutomationProperties.Name=\"다음\"/>"
        L"</StackPanel>"
        L"</Border>";
    return xaml;
}

FrameworkElement LoadWidget() {
    for (bool themed : {true, false}) {
        try {
            return Markup::XamlReader::Load(winrt::hstring(WidgetXaml(themed)))
                .as<FrameworkElement>();
        } catch (...) {
            Wh_Log(L"Media widget (%s) failed: %08X",
                   themed ? L"themed" : L"plain", winrt::to_hresult());
        }
    }
    return nullptr;
}

Ui* Build(HWND taskbar) {
    auto root = LoadWidget();
    if (!root) {
        return nullptr;
    }
    auto part = [&](PCWSTR name) { return root.FindName(name); };
    Ui ui;
    ui.taskbar = taskbar;
    ui.root = root;
    ui.info = part(L"Info").as<FrameworkElement>();
    ui.art = part(L"Art").as<Controls::Border>();
    ui.title = part(L"Title").as<Controls::TextBlock>();
    const PCWSTR names[] = {L"Previous", L"Toggle", L"Next"};
    for (int i = 0; i < 3; i++) {
        ui.buttons[i] = part(names[i]).as<Controls::Button>();
    }
    ui.title.MaxWidth(settings::g.media.maxWidth);
    const uint32_t commands[] = {kPrevious, kToggle, kNext};
    for (int i = 0; i < 3; i++) {
        uint32_t command = commands[i];
        ui.clicks[i] = ui.buttons[i].Click([command](auto&&, auto&&) {
            core::Guard(L"Media button", [&] { Signal(0, command); });
        });
    }
    return &g_uis.emplace_back(std::move(ui));
}

// Removed here, not when XAML lets go of the buttons, which may be after the
// mod is gone.
void Release(Ui& ui) {
    for (int i = 0; i < 3; i++) {
        core::Guard(L"Media button revoke", [&] { ui.buttons[i].Click(ui.clicks[i]); });
    }
    core::Guard(L"Media art release", [&] { ui.art.Background(nullptr); });
}

void SetArt(Ui& ui, wss::IRandomAccessStream const& stream) {
    if (!stream) {
        ui.art.Background(nullptr);
        return;
    }
    Media::Imaging::BitmapImage bitmap;
    // Logical pixels: XAML decodes at the shown size times the DPI scale.
    bitmap.DecodePixelType(Media::Imaging::DecodePixelType::Logical);
    bitmap.DecodePixelHeight(theme::kMediaArtSize);
    // Not awaited; a failed decode leaves the placeholder.
    bitmap.SetSourceAsync(stream);
    Media::ImageBrush brush;
    brush.Stretch(Media::Stretch::UniformToFill);
    brush.ImageSource(bitmap);
    ui.art.Background(brush);
}

void ShowOn(HWND taskbar, Snapshot const& s) {
    auto it = std::ranges::find(g_uis, taskbar, &Ui::taskbar);
    if (!s.visible) {
        widgets::Remove(taskbar, L"media");
        if (it != g_uis.end()) {
            SetArt(*it, nullptr);
        }
        return;
    }
    Ui* p = it != g_uis.end() ? &*it : Build(taskbar);
    if (!p) {
        return;
    }
    auto& ui = *p;
    ui.title.Text(s.title);
    std::wstring tip(s.title);
    if (!s.artist.empty()) {
        tip += (tip.empty() ? L"" : L"\n") + std::wstring(s.artist);
    }
    if (tip != ui.tip) {
        ui.tip = tip;
        Controls::ToolTipService::SetToolTip(
            ui.info, tip.empty() ? nullptr : box_value(winrt::hstring(tip)));
    }
    if (ui.playing != (int)s.playing) {
        ui.playing = s.playing;
        ui.buttons[1].Content(
            box_value(winrt::hstring(s.playing ? L"\uE769" : L"\uE768")));
    }
    ui.buttons[0].IsEnabled(s.canPrevious);
    ui.buttons[1].IsEnabled(s.canToggle);
    ui.buttons[2].IsEnabled(s.canNext);
    if (s.artChanged) {
        // A stream of its own: each bitmap reads from where its stream is.
        SetArt(ui, s.art ? s.art.CloneStream() : nullptr);
    }
    widgets::Add(taskbar, L"media", 10, ui.root, widgets::Side::kEnd);
}

// Drops the widgets of taskbars that are gone.
void Prune() {
    std::erase_if(g_taskbars, [](HWND hwnd) { return !IsWindow(hwnd); });
    for (auto it = g_uis.begin(); it != g_uis.end();) {
        if (std::ranges::find(g_taskbars, it->taskbar) == g_taskbars.end()) {
            Release(*it);
            it = g_uis.erase(it);
        } else {
            ++it;
        }
    }
}

void Show(Snapshot const& s) {
    if (g_taskbars.empty() || core::g_vertical) {
        return;
    }
    if (g_onShow) {
        core::Guard(L"Media island", [&] { g_onShow(s); });
    }
    Prune();
    for (HWND taskbar : g_taskbars) {
        core::Guard(L"Media widget", [&] { ShowOn(taskbar, s); });
    }
}

// ---- lifecycle --------------------------------------------------------------------

void Start() {
    if (core::g_vertical || g_thread) {
        return;
    }
    g_stop = false;
    g_wake = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_wake) {
        Wh_Log(L"Media event failed");
        return;
    }
    g_dirty = kSession | kResend;
    // Stop may have to leave the worker in a player's call; this keeps the
    // module's code loaded until the worker gets out.
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (PCWSTR)Worker, &module);
    g_thread = CreateThread(nullptr, 0, Worker, module, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"Media thread failed");
        CloseHandle(g_wake);
        g_wake = nullptr;
        if (module) {
            FreeLibrary(module);
        }
    }
}

void Apply(HWND taskbar) {
    if (core::g_vertical || !taskbar ||
        std::ranges::find(g_taskbars, taskbar) != g_taskbars.end()) {
        return;
    }
    g_taskbars.push_back(taskbar);
    // Restore dropped the widgets, and a new taskbar has none: have the
    // worker send everything again.
    Signal(kResend, 0);
}

void Restore() {
    for (HWND taskbar : g_taskbars) {
        widgets::Remove(taskbar, L"media");
    }
    g_taskbars.clear();
    for (auto& ui : g_uis) {
        Release(ui);
    }
    g_uis.clear();
}

void Stop() {
    if (!g_thread) {
        return;
    }
    g_stop = true;
    {
        std::lock_guard guard(g_opLock);
        if (g_op) {
            core::Guard(L"Media cancel", [] { g_op.Cancel(); });
        }
    }
    SetEvent(g_wake);
    // A player that ignores the cancel can hold the worker in its call for
    // good. It's left there with its handles; its module reference keeps this
    // code loaded until it gets out.
    if (WaitForSingleObject(g_thread, 3000) == WAIT_TIMEOUT) {
        Wh_Log(L"Media worker left running");
        g_thread = nullptr;
        return;
    }
    CloseHandle(g_thread);
    g_thread = nullptr;
    // The sources let go of the handlers as the worker unsubscribes, or once
    // an event they are delivering is done.
    for (ULONGLONG until = GetTickCount64() + 1000;
         (g_handlers > 0 || g_signalling > 0) && GetTickCount64() < until;) {
        Sleep(1);
    }
    if (g_handlers > 0 || g_signalling > 0) {
        Wh_Log(L"%d media handlers still held", g_handlers.load());
    }
    // A handler may still be returning from its last release.
    Sleep(50);
    CloseHandle(g_wake);
    g_wake = nullptr;
}

}  // namespace media

// The usage meter's details, built on the folder popup further down.
namespace usage_detail {
void Toggle(HWND taskbar, FrameworkElement const& anchor);
void Refresh();
void Close();
}  // namespace usage_detail

// ============================================================================
namespace usage {
// ============================================================================
// One block per account in the tray widgets: initial, weekly percent, a thin
// bar. The numbers come from the JSON file Bloom exports; a waiting thread is
// woken by changes of its folder and posts the parsed model to the taskbar
// thread. What is stale is decided when the block is drawn, with the current
// time, so nothing runs between file changes.

namespace theme = settings::theme;
namespace wdj = winrt::Windows::Data::Json;
using winrt::box_value;

constexpr PCWSTR kWidgetName = L"usage";
constexpr int kWidgetOrder = 20;
constexpr PCWSTR kFileName = L"ai-usage.json";
constexpr int kSchemaVersion = 1;
constexpr DWORD kMaxFileBytes = 64 * 1024;
constexpr size_t kMaxAccounts = 8;
constexpr size_t kMaxLimits = 4;
constexpr size_t kMaxText = 40;
constexpr int kDefaultEveryMins = 10;
constexpr int64_t kGiveUpSeconds = 24 * 3600;

// ---- model --------------------------------------------------------------------
// What the file says, no more: no addresses, tokens or paths are read.

struct Limit {
    int mins = 0;  // 0 = unknown
    double used = 0;
    int64_t resetsAt = 0;  // 0 = unknown
    // Empty in older files: "session", "week" or "week-<model>", and the
    // window's display name.
    std::wstring key, name;
};

struct Account {
    std::wstring id, kind, name, plan, label;
    // The key of the limit to show, if chosen.
    std::wstring bar;
    // A picture file of the user's for the logo; `icon` is empty when the
    // path given can't be used, which draws as a file that won't load.
    bool hasIcon = false;
    std::wstring icon;
    bool signedOut = false;
    int64_t fetchedAt = 0;
    int everyMins = kDefaultEveryMins;
    std::vector<Limit> limits;
};

// What is written after the logo: nothing, the account's name or its plan.
enum class Tag { kNone, kName, kPlan };

struct Model {
    bool online = true;
    int64_t writtenAt = 0;
    Tag tag = Tag::kNone;
    // Bloom counts what is left ("show": "left") instead of what is used.
    bool left = false;
    std::vector<Account> accounts;
};

int64_t Now() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

// ---- parsing (worker thread) ----------------------------------------------------

wdj::IJsonValue Field(wdj::JsonObject const& obj,
                      PCWSTR key,
                      wdj::JsonValueType type) {
    auto value = obj.TryLookup(key);
    return value && value.ValueType() == type ? value : nullptr;
}

double Number(wdj::JsonObject const& obj, PCWSTR key, double fallback) {
    auto value = Field(obj, key, wdj::JsonValueType::Number);
    return value ? value.GetNumber() : fallback;
}

bool Flag(wdj::JsonObject const& obj, PCWSTR key, bool fallback) {
    auto value = Field(obj, key, wdj::JsonValueType::Boolean);
    return value ? value.GetBoolean() : fallback;
}

// A name is shown on a screen that may be shared; anything with an '@' is
// dropped in case it is an address.
std::wstring Text(wdj::JsonObject const& obj, PCWSTR key) {
    auto value = Field(obj, key, wdj::JsonValueType::String);
    if (!value) {
        return {};
    }
    auto s = value.GetString();
    std::wstring out(s.c_str(), std::min<size_t>(s.size(), kMaxText));
    if (out.find(L'@') != std::wstring::npos) {
        return {};
    }
    for (auto& c : out) {
        if (c < 0x20) {
            c = L' ';
        }
    }
    return out;
}

// Only a whole path on a drive: a share could keep the taskbar waiting.
constexpr size_t kMaxPath = 1024;

bool LocalPath(std::wstring_view path) {
    return path.size() >= 3 && path.size() <= kMaxPath && iswalpha(path[0]) &&
           path[1] == L':' && (path[2] == L'\\' || path[2] == L'/') &&
           std::ranges::none_of(path, [](WCHAR c) { return c < 0x20; });
}

Tag ParseTag(std::wstring const& text) {
    return text == L"name" ? Tag::kName : text == L"plan" ? Tag::kPlan : Tag::kNone;
}

Account ParseAccount(wdj::JsonObject const& obj) {
    Account a;
    a.id = Text(obj, L"id");
    a.kind = Text(obj, L"kind");
    a.name = Text(obj, L"name");
    a.plan = Text(obj, L"plan");
    a.label = Text(obj, L"label");
    a.bar = Text(obj, L"bar");
    if (auto icon = Field(obj, L"icon", wdj::JsonValueType::String)) {
        winrt::hstring text = icon.GetString();  // `path` reads from it
        std::wstring_view path = text;
        a.hasIcon = !path.empty();
        if (LocalPath(path)) {
            a.icon = path;
        }
    }
    a.signedOut = Flag(obj, L"signedOut", false);
    a.fetchedAt = (int64_t)Number(obj, L"fetchedAt", 0);
    int every = (int)Number(obj, L"everyMins", kDefaultEveryMins);
    a.everyMins = every > 0 ? every : kDefaultEveryMins;
    if (auto limits = Field(obj, L"limits", wdj::JsonValueType::Array)) {
        for (auto const& item : limits.GetArray()) {
            if (a.limits.size() >= kMaxLimits) {
                break;
            }
            if (item.ValueType() != wdj::JsonValueType::Object) {
                continue;
            }
            auto cap = item.GetObject();
            if (!Field(cap, L"used", wdj::JsonValueType::Number)) {
                continue;
            }
            Limit l;
            l.mins = (int)Number(cap, L"mins", 0);
            l.used = Number(cap, L"used", 0);
            l.resetsAt = (int64_t)Number(cap, L"resetsAt", 0);
            l.key = Text(cap, L"key");
            l.name = Text(cap, L"name");
            a.limits.push_back(l);
        }
    }
    return a;
}

// False when `text` isn't a JSON object (a foreign or half-written file). A
// newer schema parses to a model without accounts, so the meter stays hidden.
bool Parse(std::wstring const& text, Model& model) {
    wdj::JsonObject root{nullptr};
    if (!wdj::JsonObject::TryParse(text, root)) {
        return false;
    }
    model = {};
    int version = (int)Number(root, L"v", 0);
    if (version < 1 || version > kSchemaVersion) {
        Wh_Log(L"Usage file schema %d is not supported", version);
        return true;
    }
    model.online = Flag(root, L"online", true);
    model.writtenAt = (int64_t)Number(root, L"writtenAt", 0);
    model.tag = ParseTag(Text(root, L"tag"));
    model.left = Text(root, L"show") == L"left";
    if (auto accounts = Field(root, L"accounts", wdj::JsonValueType::Array)) {
        for (auto const& item : accounts.GetArray()) {
            if (model.accounts.size() >= kMaxAccounts) {
                break;
            }
            if (item.ValueType() == wdj::JsonValueType::Object) {
                model.accounts.push_back(ParseAccount(item.GetObject()));
            }
        }
    }
    return true;
}

// ---- file and folder (worker thread) ----------------------------------------------

// The file's folder and the two above it, which are watched while the folder
// below doesn't exist yet.
constexpr PCWSTR kBloomDirName = L"Bloom";
constexpr PCWSTR kExportDirName = L"export";

std::wstring g_local, g_bloom, g_dir;
// Set once the paths are written: a starting explorer's taskbar thread may
// already read them in Apply.
std::atomic<bool> g_pathsReady;

bool InitPaths() {
    WCHAR base[MAX_PATH];
    DWORD n = GetEnvironmentVariable(L"LOCALAPPDATA", base, ARRAYSIZE(base));
    if (!n || n >= ARRAYSIZE(base)) {
        return false;
    }
    g_local = base;
    g_bloom = g_local + L"\\" + kBloomDirName;
    g_dir = g_bloom + L"\\" + kExportDirName;
    return true;
}

bool IsDirectory(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

enum class Level { kExport, kBloom, kLocal };

// The deepest of the three folders that exists.
Level Nearest() {
    return IsDirectory(g_dir)     ? Level::kExport
           : IsDirectory(g_bloom) ? Level::kBloom
                                  : Level::kLocal;
}

enum class Read { kOk, kMissing, kFailed };

// Opened with every sharing flag so Bloom can replace the file meanwhile; a
// sharing violation is retried once.
Read ReadBytes(std::string& bytes) {
    std::wstring path = g_dir + L"\\" + kFileName;
    for (int attempt = 0; attempt < 2; attempt++) {
        winrt::file_handle file{CreateFileW(
            path.c_str(), GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
        if (!file) {
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
                return Read::kMissing;
            }
            if (error == ERROR_SHARING_VIOLATION && attempt == 0) {
                Sleep(50);
                continue;
            }
            return Read::kFailed;
        }
        LARGE_INTEGER size;
        if (!GetFileSizeEx(file.get(), &size) || size.QuadPart > kMaxFileBytes) {
            return Read::kFailed;
        }
        bytes.resize((size_t)size.QuadPart);
        DWORD got = 0;
        if (!bytes.empty() &&
            !ReadFile(file.get(), bytes.data(), (DWORD)bytes.size(), &got, nullptr)) {
            return Read::kFailed;
        }
        bytes.resize(got);
        return Read::kOk;
    }
    return Read::kFailed;
}

std::wstring Widen(std::string const& bytes) {
    size_t skip = bytes.starts_with("\xEF\xBB\xBF") ? 3 : 0;
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip,
                                (int)(bytes.size() - skip), nullptr, 0);
    std::wstring text(n > 0 ? n : 0, L'\0');
    if (n > 0) {
        MultiByteToWideChar(CP_UTF8, 0, bytes.data() + skip,
                            (int)(bytes.size() - skip), text.data(), n);
    }
    return text;
}

void OnModel(std::shared_ptr<const Model> model);

// Reads the file and posts the model; unchanged bytes and unreadable or
// half-written files post nothing, so the last good model stays on screen.
void ReadAndPost(std::string& last) {
    std::string bytes;
    Read result = ReadBytes(bytes);
    if (result == Read::kFailed) {
        return;
    }
    auto model = std::make_shared<Model>();
    if (result == Read::kOk) {
        if (bytes == last || !Parse(Widen(bytes), *model)) {
            return;
        }
    } else {
        bytes.clear();  // a deleted file empties the meter
        if (last.empty()) {
            return;
        }
    }
    last = std::move(bytes);
    core::Post([model] { OnModel(model); });
}

bool Names(const BYTE* buffer, PCWSTR name) {
    for (;;) {
        auto info = (const FILE_NOTIFY_INFORMATION*)buffer;
        size_t length = info->FileNameLength / sizeof(WCHAR);
        if (length == wcslen(name) &&
            _wcsnicmp(info->FileName, name, length) == 0) {
            return true;
        }
        if (!info->NextEntryOffset) {
            return false;
        }
        buffer += info->NextEntryOffset;
    }
}

HANDLE g_thread;
HANDLE g_stop;

// One thread, one directory handle at a time. It waits on the handle and the
// stop event only. In the export folder a file event is a change of the usage
// file; while that folder (or Bloom's) is missing, the nearest existing parent
// is watched for the next folder to appear, and a folder that goes away sends
// it back up.
void Watch() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    winrt::handle changed{CreateEvent(nullptr, TRUE, FALSE, nullptr)};
    if (!changed) {
        Wh_Log(L"Usage watcher could not start");
        return;
    }
    HANDLE waits[] = {changed.get(), g_stop};
    alignas(DWORD) BYTE buffer[4096];
    std::string last;
    bool stopped = false;
    int failures = 0;  // in a row; a watcher that can't open anything gives up

    while (!stopped && failures < 3) {
        Level level = Nearest();
        std::wstring const& path = level == Level::kExport  ? g_dir
                                   : level == Level::kBloom ? g_bloom
                                                            : g_local;
        winrt::file_handle dir{CreateFileW(
            path.c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr)};
        if (!dir) {
            failures++;  // removed since Nearest()
            continue;
        }

        OVERLAPPED overlapped{};
        overlapped.hEvent = changed.get();
        bool pending = false;
        DWORD filter = level == Level::kExport
                           ? FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE
                           : FILE_NOTIFY_CHANGE_DIR_NAME;
        auto arm = [&] {
            ResetEvent(changed.get());
            pending = ReadDirectoryChangesW(dir.get(), buffer, sizeof(buffer),
                                            FALSE, filter, nullptr, &overlapped,
                                            nullptr);
            return pending;
        };

        bool again = !arm();  // leave to the outer loop: re-evaluate the level
        if (again) {
            failures++;
        }
        // Armed before the check and the first read, so a folder or file that
        // appears in between isn't lost.
        if (!again && Nearest() != level) {
            again = true;
        } else if (!again && level == Level::kExport) {
            ReadAndPost(last);
        }
        while (!again) {
            if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0) {
                stopped = true;
                break;
            }
            DWORD bytes = 0;
            pending = false;
            if (!GetOverlappedResult(dir.get(), &overlapped, &bytes, FALSE)) {
                failures++;  // the folder is gone
                break;
            }
            failures = 0;
            // Zero bytes = the buffer overflowed: something changed, not what.
            bool hit = bytes == 0 ||
                       Names(buffer, level == Level::kExport  ? kFileName
                                     : level == Level::kBloom ? kExportDirName
                                                              : kBloomDirName);
            if (hit && level != Level::kExport) {
                break;  // the folder below appeared (or changed): move down
            }
            if (!arm()) {
                failures++;
                break;
            }
            if (hit) {
                ReadAndPost(last);
            }
        }
        if (pending) {
            CancelIoEx(dir.get(), &overlapped);
            DWORD bytes;
            GetOverlappedResult(dir.get(), &overlapped, &bytes, TRUE);
        }
        if (!stopped && level == Level::kExport && Nearest() != level &&
            !last.empty()) {
            last.clear();  // the export folder is gone: the meter goes with it
            core::Post([] { OnModel(nullptr); });
        }
    }
    if (!stopped) {
        Wh_Log(L"Usage watcher ended");
    }
}

DWORD WINAPI WatchProc(void*) {
    core::Guard(L"Usage watcher", Watch);
    return 0;
}

// ---- what to show (taskbar thread) --------------------------------------------------

struct Label {
    std::wstring initial, title;
    std::wstring tag;     // after the logo; empty for none
    std::wstring letter;  // on the tile of an icon that can't be drawn
};

// The tile's letter: the kind's first, but G for ChatGPT, whose C is Claude's.
std::wstring TileLetter(std::wstring const& kind) {
    if (kind == L"chatgpt") {
        return L"G";
    }
    return std::wstring(1, kind.empty() ? L'?' : (WCHAR)towupper(kind[0]));
}

std::wstring TagText(Tag tag, Account const& a) {
    switch (tag) {
        case Tag::kName:
            return !a.label.empty() ? a.label : a.name;
        case Tag::kPlan:
            return a.plan;
        default:
            return {};
    }
}

// "C", "G"; a second account of one kind is "C2". The title is the alias the
// user gave the account, else the provider name.
std::vector<Label> Labels(Model const& model) {
    std::vector<Label> labels;
    std::unordered_map<std::wstring, int> seen;
    for (auto const& a : model.accounts) {
        int n = ++seen[a.kind];
        std::wstring base = !a.name.empty() ? a.name : a.kind;
        std::wstring suffix = n > 1 ? std::to_wstring(n) : L"";
        Label label;
        label.initial =
            std::wstring(1, base.empty() ? L'?' : (WCHAR)towupper(base[0])) + suffix;
        label.title = !a.label.empty()
                          ? a.label
                          : base + (n > 1 ? L" " + suffix : L"");
        label.tag = TagText(model.tag, a);
        label.letter = TileLetter(a.kind);
        labels.push_back(std::move(label));
    }
    return labels;
}

int64_t Age(Account const& a, int64_t now) {
    // A time in the future (clock moved back) counts as just fetched.
    return a.fetchedAt > 0 ? std::max<int64_t>(0, now - a.fetchedAt)
                           : kGiveUpSeconds + 1;
}

int64_t FreshSeconds(Account const& a) {
    return 2 * a.everyMins * 60 + 120;
}

bool Fresh(Model const& m, Account const& a, int64_t now) {
    return m.online && !a.signedOut && Age(a, now) <= FreshSeconds(a);
}

// The weekly limit: in a file with keys the one keyed "week" (all models),
// never a model's own week, else the longest unkeyed window of a week or
// more; in an older file the longest window, the later one on a tie.
const Limit* Weekly(Account const& a) {
    bool keyed =
        std::ranges::any_of(a.limits, [](Limit const& l) { return !l.key.empty(); });
    if (keyed) {
        auto it = std::ranges::find(a.limits, std::wstring_view(L"week"), &Limit::key);
        if (it != a.limits.end()) {
            return &*it;
        }
    }
    constexpr int kWeekMins = 7 * 24 * 60;
    const Limit* best = nullptr;
    for (auto const& l : a.limits) {
        if (keyed && (!l.key.empty() || l.mins < kWeekMins)) {
            continue;
        }
        if (!best || l.mins >= best->mins) {
            best = &l;
        }
    }
    return best;
}

// The limit chosen for the account, else the weekly one.
const Limit* BarLimit(Account const& a) {
    if (!a.bar.empty()) {
        auto it = std::ranges::find(a.limits, a.bar, &Limit::key);
        if (it != a.limits.end()) {
            return &*it;
        }
    }
    return Weekly(a);
}

// Whether the numbers count what is left: Bloom's choice, until a right-click on
// the meter turns it the other way here. A new choice of Bloom's comes first again.
std::optional<bool> g_leftFlip;

bool ShowsLeft(Model const& m) {
    return g_leftFlip.value_or(m.left);
}

struct Value {
    enum Kind { kNumber, kUnknown, kNone } kind = kNone;
    double percent = 0;  // used; the alert colors go by it either way
    bool left = false;   // the number shown is what is left

    // The whole percent shown: used, or what is left; the two add up to 100.
    int Shown() const {
        int used = (int)std::lround(percent);
        return left ? std::max(0, 100 - used) : used;
    }

    // The number alone; Text() has its unit.
    std::wstring Figure() const {
        switch (kind) {
            case kNumber:
                return std::to_wstring(Shown());
            case kUnknown:
                return L"?";
            default:
                return L"–";
        }
    }

    std::wstring Text() const {
        return kind == kNumber ? Figure() + L"\u00A0%" + (left ? L" 남음" : L"") : Figure();
    }
};

// A number from an old fetch is hidden after a day; once its window has reset
// the new window's use is unknown, which is 0 only while the data is fresh.
Value Evaluate(Model const& m, Account const& a, Limit const* l, int64_t now) {
    Value v;
    v.left = ShowsLeft(m);
    if (!l || Age(a, now) > kGiveUpSeconds) {
        return v;
    }
    if (l->resetsAt > 0 && l->resetsAt <= now) {
        v.kind = Fresh(m, a, now) ? Value::kNumber : Value::kUnknown;
        return v;
    }
    v.kind = Value::kNumber;
    v.percent = std::clamp(l->used, 0.0, 100.0);
    return v;
}

bool Dim(Model const& m, Account const& a, int64_t now) {
    return !Fresh(m, a, now);
}

std::wstring Duration(int64_t seconds) {
    int64_t mins = seconds / 60;
    int64_t days = mins / 1440, hours = mins % 1440 / 60;
    if (days > 0) {
        return std::to_wstring(days) + L"일" +
               (hours ? L" " + std::to_wstring(hours) + L"시간" : L"");
    }
    if (hours > 0) {
        return std::to_wstring(hours) + L"시간" +
               (mins % 60 ? L" " + std::to_wstring(mins % 60) + L"분" : L"");
    }
    return std::to_wstring(mins) + L"분";
}

std::wstring WindowName(int mins) {
    if (mins == 300) {
        return L"5시간";
    }
    if (mins == 10080) {
        return L"주간";
    }
    if (mins > 0 && mins % 1440 == 0) {
        return std::to_wstring(mins / 1440) + L"일";
    }
    if (mins > 0 && mins % 60 == 0) {
        return std::to_wstring(mins / 60) + L"시간";
    }
    return mins > 0 ? std::to_wstring(mins) + L"분" : L"한도";
}

std::wstring Tooltip(Model const& m,
                     Account const& a,
                     Label const& label,
                     int64_t now) {
    std::wstring text = label.title;
    if (!a.plan.empty()) {
        text += L" · " + a.plan;
    }
    for (auto const& l : a.limits) {
        Value v = Evaluate(m, a, &l, now);
        text += L"\n" + (l.name.empty() ? WindowName(l.mins) : l.name) + L" " + v.Text();
        if (v.kind == Value::kNumber && l.resetsAt > now) {
            text += L" · " + Duration(l.resetsAt - now) + L" 뒤 리셋";
        }
    }
    if (a.signedOut) {
        text += L"\n로그인 필요";
    } else if (!m.online) {
        text += L"\nBloom 꺼짐 · " +
                Duration(std::max<int64_t>(0, now - m.writtenAt)) + L" 전";
    } else if (Age(a, now) > kGiveUpSeconds) {
        text += L"\n하루 넘게 갱신 없음";
    } else {
        text += L"\n" + Duration(Age(a, now)) + L" 전 갱신" +
                (Fresh(m, a, now) ? L"" : L" (오래됨)");
    }
    return text;
}

// ---- account icons (taskbar thread) ---------------------------------------------------
// A picture file the user chose as an account's logo, read once per change of
// the file and drawn at each size asked for. What is kept is plain pixels and
// bytes, so the cache holds no COM object.

constexpr DWORD kMaxSvgBytes = 1024 * 1024;
constexpr uint64_t kMaxPictureBytes = 8 * 1024 * 1024;
constexpr UINT kMaxPictureSide = 256;  // a larger picture is kept scaled to this
constexpr UINT kMaxIconSide = 256;

// Premultiplied BGRA, `size` square, the picture centered in it.
struct IconPixels {
    UINT size = 0;
    std::vector<uint32_t> bgra;
};

struct FileStamp {
    bool exists = false;
    FILETIME written{};
    uint64_t bytes = 0;

    bool operator==(FileStamp const& o) const {
        return exists == o.exists && bytes == o.bytes &&
               CompareFileTime(&written, &o.written) == 0;
    }
};

struct IconFile {
    std::wstring path;
    FileStamp stamp;
    std::string svg;  // the document, for an .svg
    UINT width = 0, height = 0;
    std::vector<uint32_t> picture;  // else the picture, premultiplied BGRA
    // By size in pixels; null where it couldn't be drawn.
    std::vector<std::pair<UINT, std::shared_ptr<const IconPixels>>> sizes;
};

[[clang::no_destroy]] std::list<IconFile> g_icons;
// Counts files read again, for what was drawn from the ones before.
unsigned g_iconsRead;

FileStamp Stamp(std::wstring const& path) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (path.empty() ||
        !GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data) ||
        (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return {};
    }
    return {true, data.ftLastWriteTime,
            (uint64_t)data.nFileSizeHigh << 32 | data.nFileSizeLow};
}

bool IsSvg(std::wstring const& path) {
    return path.size() > 4 && _wcsicmp(path.c_str() + path.size() - 4, L".svg") == 0;
}

bool ReadSvg(IconFile& f) {
    winrt::file_handle file{CreateFileW(f.path.c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    LARGE_INTEGER size;
    if (!file || !GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > kMaxSvgBytes) {
        return false;
    }
    f.svg.resize((size_t)size.QuadPart);
    DWORD got = 0;
    if (!ReadFile(file.get(), f.svg.data(), (DWORD)f.svg.size(), &got, nullptr)) {
        return false;
    }
    f.svg.resize(got);
    return got > 0;
}

// A source scaled to fit `side` square, smoothly either way.
winrt::com_ptr<IWICBitmapSource> Scaled(IWICImagingFactory* wic,
                                        IWICBitmapSource* source,
                                        UINT width,
                                        UINT height,
                                        UINT side) {
    float k = (float)side / std::max(width, height);
    UINT w = std::max<UINT>(1, (UINT)std::lround(width * k));
    UINT h = std::max<UINT>(1, (UINT)std::lround(height * k));
    winrt::com_ptr<IWICBitmapScaler> scaler;
    if (FAILED(wic->CreateBitmapScaler(scaler.put())) ||
        FAILED(scaler->Initialize(source, w, h,
                                  k < 1 ? WICBitmapInterpolationModeFant
                                        : WICBitmapInterpolationModeCubic))) {
        return nullptr;
    }
    return scaler;
}

// A .png, .ico or any picture WIC reads; of an .ico's sizes the largest.
bool ReadPicture(IconFile& f) {
    if (f.stamp.bytes > kMaxPictureBytes) {
        return false;
    }
    auto wic = winrt::try_create_instance<IWICImagingFactory>(CLSID_WICImagingFactory);
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    UINT frames = 0;
    if (!wic ||
        FAILED(wic->CreateDecoderFromFilename(f.path.c_str(), nullptr, GENERIC_READ,
                                              WICDecodeMetadataCacheOnDemand, decoder.put())) ||
        FAILED(decoder->GetFrameCount(&frames))) {
        return false;
    }
    winrt::com_ptr<IWICBitmapFrameDecode> best;
    uint64_t bestArea = 0;
    for (UINT i = 0; i < std::min<UINT>(frames, 32); i++) {
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        UINT w = 0, h = 0;
        if (SUCCEEDED(decoder->GetFrame(i, frame.put())) && SUCCEEDED(frame->GetSize(&w, &h)) &&
            (uint64_t)w * h > bestArea) {
            best = frame;
            bestArea = (uint64_t)w * h;
        }
    }
    winrt::com_ptr<IWICBitmapSource> source;
    UINT w = 0, h = 0;
    if (!best ||
        FAILED(WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, best.get(), source.put())) ||
        FAILED(source->GetSize(&w, &h)) || !w || !h) {
        return false;
    }
    if (std::max(w, h) > kMaxPictureSide) {
        source = Scaled(wic.get(), source.get(), w, h, kMaxPictureSide);
        if (!source || FAILED(source->GetSize(&w, &h))) {
            return false;
        }
    }
    f.picture.resize((size_t)w * h);
    if (FAILED(source->CopyPixels(nullptr, w * 4, (UINT)f.picture.size() * 4,
                                  (BYTE*)f.picture.data()))) {
        f.picture.clear();
        return false;
    }
    f.width = w;
    f.height = h;
    return true;
}

IconFile ReadIconFile(std::wstring const& path) {
    IconFile f;
    f.path = path;
    f.stamp = Stamp(path);
    if (f.stamp.exists && !(IsSvg(path) ? ReadSvg(f) : ReadPicture(f))) {
        f.svg.clear();
        Wh_Log(L"Usage icon could not be read");
    }
    return f;
}

// An <svg> drawn as an <img> is, in the `px` square: with a view box its own
// width and height give way to the square; without one its width and height
// are scaled into it, centered. Returns the transform to draw it with.
// Attribute setters taking a value fail (E_INVALIDARG) as this compiler's
// headers declare them, so only removal and the transform are used.
D2D1_MATRIX_3X2_F FitSvg(ID2D1SvgDocument* doc, UINT px) {
    auto fit = D2D1::Matrix3x2F::Identity();
    winrt::com_ptr<ID2D1SvgElement> root;
    doc->GetRoot(root.put());
    if (!root) {
        return fit;
    }
    if (root->IsAttributeSpecified(L"viewBox")) {
        root->RemoveAttribute(L"width");
        root->RemoveAttribute(L"height");
        return fit;
    }
    D2D1_SVG_LENGTH w{}, h{};
    if (SUCCEEDED(root->GetAttributeValue(L"width", D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, &w,
                                          sizeof(w))) &&
        SUCCEEDED(root->GetAttributeValue(L"height", D2D1_SVG_ATTRIBUTE_POD_TYPE_LENGTH, &h,
                                          sizeof(h))) &&
        w.units == D2D1_SVG_LENGTH_UNITS_NUMBER && h.units == D2D1_SVG_LENGTH_UNITS_NUMBER &&
        w.value > 0 && h.value > 0) {
        float k = px / std::max(w.value, h.value);
        fit = D2D1::Matrix3x2F::Scale(k, k) *
              D2D1::Matrix3x2F::Translation((px - w.value * k) / 2, (px - h.value * k) / 2);
        doc->SetViewportSize(D2D1::SizeF(w.value, h.value));
    }
    return fit;
}

// Through a render target on a WIC bitmap, whose device context draws SVG
// from Windows 10 1703 on.
bool DrawSvg(IconFile const& f, IWICImagingFactory* wic, IconPixels& out) {
    UINT px = out.size;
    winrt::com_ptr<ID2D1Factory1> d2d;
    D2D1_FACTORY_OPTIONS options{};
    winrt::com_ptr<IWICBitmap> target;
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1),
                                 &options, d2d.put_void())) ||
        FAILED(wic->CreateBitmap(px, px, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad,
                                 target.put()))) {
        return false;
    }
    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    winrt::com_ptr<ID2D1RenderTarget> rt;
    if (FAILED(d2d->CreateWicBitmapRenderTarget(target.get(), &props, rt.put()))) {
        return false;
    }
    auto dc = rt.try_as<ID2D1DeviceContext5>();
    winrt::com_ptr<IStream> stream;
    stream.attach(SHCreateMemStream((const BYTE*)f.svg.data(), (UINT)f.svg.size()));
    winrt::com_ptr<ID2D1SvgDocument> doc;
    if (!dc || !stream ||
        FAILED(dc->CreateSvgDocument(stream.get(), D2D1::SizeF((float)px, (float)px),
                                     doc.put()))) {
        return false;
    }
    auto fit = FitSvg(doc.get(), px);
    dc->BeginDraw();
    D2D1_COLOR_F clear{};
    dc->Clear(&clear);
    dc->SetTransform(fit);
    dc->DrawSvgDocument(doc.get());
    if (FAILED(dc->EndDraw())) {
        return false;
    }
    return SUCCEEDED(target->CopyPixels(nullptr, px * 4, px * px * 4, (BYTE*)out.bgra.data()));
}

bool DrawPicture(IconFile const& f, IWICImagingFactory* wic, IconPixels& out) {
    winrt::com_ptr<IWICBitmap> picture;
    if (FAILED(wic->CreateBitmapFromMemory(f.width, f.height, GUID_WICPixelFormat32bppPBGRA,
                                           f.width * 4, (UINT)f.picture.size() * 4,
                                           (BYTE*)f.picture.data(), picture.put()))) {
        return false;
    }
    auto scaled = Scaled(wic, picture.get(), f.width, f.height, out.size);
    UINT w = 0, h = 0;
    if (!scaled || FAILED(scaled->GetSize(&w, &h))) {
        return false;
    }
    std::vector<uint32_t> rows((size_t)w * h);
    if (FAILED(scaled->CopyPixels(nullptr, w * 4, (UINT)rows.size() * 4, (BYTE*)rows.data()))) {
        return false;
    }
    UINT left = (out.size - w) / 2, top = (out.size - h) / 2;
    for (UINT y = 0; y < h; y++) {
        std::copy_n(&rows[(size_t)y * w], w, &out.bgra[(size_t)(top + y) * out.size + left]);
    }
    return true;
}

// The account's icon at `px` square, or null when there is none to draw.
std::shared_ptr<const IconPixels> IconAt(std::wstring const& path, UINT px) {
    if (path.empty() || !px || px > kMaxIconSide) {
        return nullptr;
    }
    auto it = std::ranges::find(g_icons, path, &IconFile::path);
    IconFile& f = it != g_icons.end() ? *it : g_icons.emplace_back(ReadIconFile(path));
    if (f.svg.empty() && f.picture.empty()) {
        return nullptr;
    }
    auto size = std::ranges::find(f.sizes, px, &decltype(f.sizes)::value_type::first);
    if (size != f.sizes.end()) {
        return size->second;
    }
    auto out = std::make_shared<IconPixels>();
    out->size = px;
    out->bgra.assign((size_t)px * px, 0);
    auto wic = winrt::try_create_instance<IWICImagingFactory>(CLSID_WICImagingFactory);
    bool ok = wic && (!f.svg.empty() ? DrawSvg(f, wic.get(), *out)
                                     : DrawPicture(f, wic.get(), *out));
    if (!ok) {
        Wh_Log(L"Usage icon could not be drawn at %u px", px);
        out = nullptr;
    }
    f.sizes.emplace_back(px, out);
    return out;
}

// Read again what changed on disk, and forget icons no account has now; a
// file that went away draws as missing from here on.
void SyncIcons(Model const* model) {
    std::erase_if(g_icons, [&](IconFile const& f) {
        return !model || std::ranges::none_of(model->accounts, [&](Account const& a) {
                   return a.icon == f.path;
               });
    });
    for (auto& f : g_icons) {
        if (!(Stamp(f.path) == f.stamp)) {
            f = ReadIconFile(f.path);
            g_iconsRead++;
        }
    }
}

// In one color, kept by the alpha, for the single-color logos.
std::shared_ptr<const IconPixels> Tinted(std::shared_ptr<const IconPixels> const& icon,
                                         winrt::Windows::UI::Color color) {
    auto out = std::make_shared<IconPixels>(*icon);
    for (auto& p : out->bgra) {
        uint32_t a = (p >> 24) * color.A / 255;
        p = a << 24 | (color.R * a / 255) << 16 | (color.G * a / 255) << 8 | color.B * a / 255;
    }
    return out;
}

// What stands for the account before its number: its icon file, the letter
// tile when that won't draw, else the letter.
enum class Mark { kInitial, kIcon, kTile };

Mark MarkOf(Account const& a, bool iconDrawn) {
    if (a.hasIcon) {
        return iconDrawn ? Mark::kIcon : Mark::kTile;
    }
    return Mark::kInitial;
}

// ---- UI (taskbar thread) -------------------------------------------------------------

struct View {
    std::wstring id;
    std::wstring mark;  // what the mark was made from, to rebuild when it changes
    Controls::StackPanel root{nullptr};
    Controls::TextBlock initial{nullptr};  // the letter; or the account's
    Controls::Image icon{nullptr};         // own icon,
    Controls::Border tile{nullptr};        // or its letter on a tile
    Controls::TextBlock tag{nullptr};
    Controls::TextBlock percent{nullptr};
    Controls::TextBlock unit{nullptr};
    Controls::Grid bar{nullptr};
    Controls::Border track{nullptr};
    Controls::Border fill{nullptr};
    std::wstring iconDrawn;  // the icon's path, size and color in `icon`
};

// One per taskbar: a XAML element lives in one tree.
struct Ui {
    HWND taskbar = nullptr;
    Controls::Border root{nullptr};
    Controls::StackPanel panel{nullptr};
    std::vector<View> views;
    UIElement::PointerEntered_revoker pointerEntered;
    FrameworkElement::ActualThemeChanged_revoker themeChanged;
    FrameworkElement::Loaded_revoker loaded;
    UIElement::Tapped_revoker tapped;
    UIElement::RightTapped_revoker rightTapped;
    UIElement::ContextRequested_revoker contextRequested;
    UIElement::PointerPressed_revoker pressed;
    UIElement::PointerReleased_revoker released;
};

[[clang::no_destroy]] std::list<Ui> g_uis;
std::vector<HWND> g_taskbars;  // applied, between Apply and Restore
std::shared_ptr<const Model> g_model;
// The island (F8) draws the same model; set between its Apply and Restore.
void (*g_onModel)() = nullptr;

theme::UsagePalette const& Palette(bool dark) {
    return dark ? theme::kUsageDark : theme::kUsageLight;
}

// Amber past the warning line, red past the danger line; none below it or
// without a number.
std::optional<winrt::Windows::UI::Color> AlertColor(Value const& v, bool dark, bool text) {
    if (v.kind != Value::kNumber || v.percent < theme::kUsageWarnPercent) {
        return std::nullopt;
    }
    auto const& p = Palette(dark);
    if (v.percent < theme::kUsageDangerPercent) {
        return p.warn;
    }
    return text ? p.dangerText : p.danger;
}

winrt::Windows::UI::Color FigureColor(Value const& v, bool dark) {
    auto const& p = Palette(dark);
    return v.kind == Value::kNone ? p.faint : AlertColor(v, dark, true).value_or(p.text);
}

// The one color of an icon when the logo color is mono.
winrt::Windows::UI::Color MonoTint(bool dark) {
    return Palette(dark).muted;
}

// Every pixel more than half there of one grey (black, grey or white, one of
// them): a one-color mark, which its company draws in the text color of a
// dark page. Black on a white disc is two.
bool OneGrey(IconPixels const& icon) {
    int low = 255, high = 0;
    bool any = false;
    for (uint32_t p : icon.bgra) {
        uint32_t a = p >> 24;
        if (a < 128) {
            continue;
        }
        int r = (int)(((p >> 16) & 0xff) * 255 / a), g = (int)(((p >> 8) & 0xff) * 255 / a),
            b = (int)((p & 0xff) * 255 / a);
        int most = std::max({r, g, b}), least = std::min({r, g, b});
        if (most - least > 24) {
            return false;  // a hue
        }
        low = std::min(low, least);
        high = std::max(high, most);
        any = true;
    }
    return any && high - low <= 48;
}

// The one color an icon is drawn in when it is drawn in one: mono's, or the
// text's for a one-color mark under the brand colors.
winrt::Windows::UI::Color IconTint(bool mono, bool dark) {
    return mono ? MonoTint(dark) : Palette(dark).text;
}

// The icon as the logo color draws it: in `tint` when mono or a one-color
// mark, else in its own colors.
std::shared_ptr<const IconPixels> Colored(std::shared_ptr<const IconPixels> const& icon,
                                          bool mono,
                                          winrt::Windows::UI::Color tint) {
    return mono || OneGrey(*icon) ? Tinted(icon, tint) : icon;
}

// The tile's color: the text's, faint.
winrt::Windows::UI::Color TileFill(bool dark) {
    auto c = Palette(dark).text;
    c.A = theme::kUsageTileAlpha;
    return c;
}

// The account's icon at the logo's size on this taskbar.
UINT IconSize(HWND taskbar) {
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 0;
    return (UINT)std::lround(theme::kUsageLogoSize * (dpi ? dpi : 96) / 96);
}

Media::Imaging::WriteableBitmap IconBitmap(IconPixels const& icon) {
    Media::Imaging::WriteableBitmap bitmap(icon.size, icon.size);
    auto buffer = bitmap.PixelBuffer();
    winrt::com_ptr<::Windows::Storage::Streams::IBufferByteAccess> access;
    BYTE* bytes = nullptr;
    size_t length = icon.bgra.size() * 4;
    if (buffer.Capacity() < length ||
        FAILED(reinterpret_cast<::IUnknown*>(winrt::get_abi(buffer))
                   ->QueryInterface(winrt::guid_of<::Windows::Storage::Streams::IBufferByteAccess>(),
                                    access.put_void())) ||
        FAILED(access->Buffer(&bytes))) {
        return nullptr;
    }
    memcpy(bytes, icon.bgra.data(), length);
    buffer.Length((uint32_t)length);
    bitmap.Invalidate();
    return bitmap;
}

// Row: the mark and the tag at the left, the number at the right of a column
// that holds "100 %", so only the tag's text moves the width; under it the
// bar, split into the used and the rest.
View MakeView(std::wstring const& id, std::wstring const& markKey, Mark markKind) {
    std::wstring mark;
    switch (markKind) {
        case Mark::kIcon:
            mark = L"<Image x:Name=\"Mark\" Stretch=\"Uniform\"/>";
            break;
        case Mark::kTile:
            mark = L"<Border x:Name=\"Mark\"><TextBlock x:Name=\"Letter\" "
                   L"FontWeight=\"SemiBold\" HorizontalAlignment=\"Center\" "
                   L"VerticalAlignment=\"Center\"/></Border>";
            break;
        default:
            mark = L"<TextBlock x:Name=\"Initial\" FontWeight=\"SemiBold\" "
                   L"VerticalAlignment=\"Center\"/>";
    }
    std::wstring radius = std::to_wstring(theme::kUsageBarRadius);
    std::wstring xaml =
        L"<StackPanel xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        L"presentation\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\""
        L" VerticalAlignment=\"Center\"><Grid x:Name=\"Row\"><Grid.ColumnDefinitions>"
        L"<ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/>"
        L"</Grid.ColumnDefinitions><StackPanel Orientation=\"Horizontal\" "
        L"VerticalAlignment=\"Center\">" +
        mark +
        L"<TextBlock x:Name=\"Tag\" VerticalAlignment=\"Center\" "
        L"TextTrimming=\"CharacterEllipsis\" Visibility=\"Collapsed\"/></StackPanel>"
        L"<StackPanel Grid.Column=\"1\" Orientation=\"Horizontal\" "
        L"HorizontalAlignment=\"Right\" VerticalAlignment=\"Center\"><TextBlock "
        L"x:Name=\"Percent\" FontWeight=\"Medium\"/><TextBlock x:Name=\"Unit\"/>"
        L"</StackPanel></Grid><Grid x:Name=\"Bar\"><Grid.ColumnDefinitions>"
        L"<ColumnDefinition/><ColumnDefinition/>"
        L"</Grid.ColumnDefinitions><Border x:Name=\"Track\" Grid.ColumnSpan=\"2\" "
        L"CornerRadius=\"" +
        radius + L"\"/><Border x:Name=\"Fill\" CornerRadius=\"" + radius +
        L"\"/></Grid></StackPanel>";

    View v;
    v.id = id;
    v.mark = markKey;
    v.root = Markup::XamlReader::Load(xaml).as<Controls::StackPanel>();
    // As wide as the block without a tag; the number's column is what is
    // left of that by the logo.
    v.root.MinWidth(theme::kUsageBlockWidth);
    v.root.Spacing(theme::kUsageLineGap);
    auto row = v.root.FindName(L"Row").as<Controls::Grid>();
    row.Height(theme::kUsageRowHeight);
    row.ColumnDefinitions().GetAt(1).MinWidth(theme::kUsageBlockWidth - theme::kUsageLogoSize);
    v.bar = v.root.FindName(L"Bar").as<Controls::Grid>();
    v.bar.Height(theme::kUsageBarHeight);
    Media::FontFamily font{theme::kUsageFont};
    if (markKind == Mark::kInitial) {
        v.initial = v.root.FindName(L"Initial").as<Controls::TextBlock>();
        v.initial.FontSize(theme::kUsageInitialSize);
        v.initial.FontFamily(font);
    } else {
        auto box = v.root.FindName(L"Mark").as<FrameworkElement>();
        box.Width(theme::kUsageLogoSize);
        box.Height(theme::kUsageLogoSize);
        box.VerticalAlignment(VerticalAlignment::Center);
        if (markKind == Mark::kIcon) {
            v.icon = box.as<Controls::Image>();
        } else {
            v.tile = box.as<Controls::Border>();
            v.tile.CornerRadius(CornerRadiusHelper::FromUniformRadius(
                theme::kUsageLogoSize * theme::kUsageTileRadius));
            auto letter = v.root.FindName(L"Letter").as<Controls::TextBlock>();
            letter.FontSize(std::max(7.0, std::round(theme::kUsageLogoSize *
                                                     theme::kUsageTileLetter)));
            letter.FontFamily(font);
        }
    }
    v.tag = v.root.FindName(L"Tag").as<Controls::TextBlock>();
    v.tag.Margin({theme::kUsageTagGap, 0, theme::kUsageTagGap, 0});
    v.tag.MaxWidth(theme::kUsageTagMaxEm * theme::kUsagePercentSize);
    v.percent = v.root.FindName(L"Percent").as<Controls::TextBlock>();
    v.unit = v.root.FindName(L"Unit").as<Controls::TextBlock>();
    v.track = v.root.FindName(L"Track").as<Controls::Border>();
    v.fill = v.root.FindName(L"Fill").as<Controls::Border>();
    for (auto const& text : {v.tag, v.percent, v.unit}) {
        text.FontSize(theme::kUsagePercentSize);
        text.FontFamily(font);
    }
    // Digits of one width, so the number keeps its place as it changes.
    Documents::Typography::SetNumeralAlignment(v.percent, FontNumeralAlignment::Tabular);
    v.unit.Text(theme::kUsageUnit);
    return v;
}

void Refresh(View& v,
             Model const& m,
             Account const& a,
             Label const& label,
             int64_t now,
             bool dark,
             UINT iconPx) {
    Value value = Evaluate(m, a, BarLimit(a), now);
    // The bar fills with the number: growing as it is used, or shrinking with what is left.
    double fraction = value.kind == Value::kNumber ? value.Shown() / 100.0 : 0;
    auto const& p = Palette(dark);
    if (v.icon) {
        // Its own colors, or one color.
        bool mono = settings::g.usage.logoColor != settings::LogoColor::kBrand;
        auto color = IconTint(mono, dark);
        std::wstring key = a.icon + L"|" + std::to_wstring(g_iconsRead) + L"|" +
                           std::to_wstring(iconPx) + L"|" + (mono ? L"m" : L"b") +
                           std::to_wstring(color.R << 16 | color.G << 8 | color.B);
        if (key != v.iconDrawn) {
            if (auto icon = IconAt(a.icon, iconPx)) {
                if (auto bitmap = IconBitmap(*Colored(icon, mono, color))) {
                    v.icon.Source(bitmap);
                    v.iconDrawn = key;
                }
            }
        }
    } else if (v.tile) {
        v.tile.Background(Media::SolidColorBrush(TileFill(dark)));
        auto letter = v.tile.Child().as<Controls::TextBlock>();
        letter.Text(label.letter);
        letter.Foreground(Media::SolidColorBrush(p.text));
    } else {
        v.initial.Text(label.initial);
        v.initial.Foreground(Media::SolidColorBrush(p.text));
    }
    v.tag.Text(label.tag);
    v.tag.Visibility(label.tag.empty() ? Visibility::Collapsed : Visibility::Visible);
    v.tag.Foreground(Media::SolidColorBrush(p.text));
    v.percent.Text(value.Figure());
    auto figure = Media::SolidColorBrush(FigureColor(value, dark));
    v.percent.Foreground(figure);
    v.unit.Visibility(value.kind == Value::kNumber ? Visibility::Visible
                                                   : Visibility::Collapsed);
    v.unit.Foreground(figure);
    v.track.Background(Media::SolidColorBrush(p.track));
    // Shares of the block, whatever its width; a used limit never shows as
    // an empty bar, at least two pixels of the narrowest block.
    if (fraction > 0) {
        fraction = std::max(fraction, 2 / theme::kUsageBlockWidth);
    }
    auto columns = v.bar.ColumnDefinitions();
    columns.GetAt(0).Width({fraction, GridUnitType::Star});
    columns.GetAt(1).Width({1 - fraction, GridUnitType::Star});
    auto fill = p.text;
    fill.A = theme::kUsageFillAlpha;
    v.fill.Background(Media::SolidColorBrush(AlertColor(value, dark, false).value_or(fill)));
    v.root.Opacity(Dim(m, a, now) ? theme::kUsageDimOpacity : 1.0);
    Controls::ToolTipService::SetToolTip(
        v.root, box_value(Tooltip(m, a, label, now)));
}

// Redraws every block with the current time, without rebuilding anything.
void RefreshAll(Ui& ui);

// A right-click on the meter: the numbers count the other way, on every
// taskbar and the island.
void FlipLeft() {
    if (!g_model) {
        return;
    }
    g_leftFlip = !ShowsLeft(*g_model);
    for (auto& ui : g_uis) {
        RefreshAll(ui);
    }
    if (g_onModel) {
        core::Guard(L"Usage island", g_onModel);
    }
}

void RefreshAll(Ui& ui) {
    if (!ui.root || !g_model || ui.views.size() != g_model->accounts.size()) {
        return;
    }
    auto const& m = *g_model;
    auto labels = Labels(m);
    int64_t now = Now();
    bool dark = ui.root.ActualTheme() == ElementTheme::Dark;
    UINT iconPx = IconSize(ui.taskbar);
    for (size_t i = 0; i < ui.views.size(); i++) {
        Refresh(ui.views[i], m, m.accounts[i], labels[i], now, dark, iconPx);
    }
}

void EnsureRoot(Ui& ui) {
    if (ui.root) {
        return;
    }
    std::wstring xaml =
        L"<Border xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        L"presentation\" VerticalAlignment=\"Center\">"
        L"<StackPanel Orientation=\"Horizontal\"/></Border>";
    auto root = Markup::XamlReader::Load(xaml).as<Controls::Border>();
    // gapAfter counts from the meter to the next tray icon, so its panel's own margin comes off.
    auto const& u = settings::g.usage;
    root.Margin({double(u.gapBefore), 0, u.gapAfter - widgets::kHostGap, 0});
    ui.panel = root.Child().as<Controls::StackPanel>();
    ui.panel.Spacing(theme::kUsageGap);
    // The tooltip's numbers are drawn again just before it can open. The list
    // keeps `ui` in place, and the revokers go with it.
    Ui* p = &ui;
    ui.pointerEntered = root.PointerEntered(
        winrt::auto_revoke, [p](auto&&, auto&&) {
            core::Guard(L"Usage hover", [p] { RefreshAll(*p); });
        });
    ui.themeChanged = root.ActualThemeChanged(
        winrt::auto_revoke, [p](auto&&, auto&&) {
            core::Guard(L"Usage theme", [p] { RefreshAll(*p); });
        });
    // The colors follow the theme; outside the taskbar's tree it isn't the
    // taskbar's yet.
    ui.loaded = root.Loaded(winrt::auto_revoke, [p](auto&&, auto&&) {
        core::Guard(L"Usage loaded", [p] { RefreshAll(*p); });
    });
    // Hit-testable between the blocks too. A click opens the details; a right-click
    // turns the numbers, and its press, release and menu stop here, so the
    // taskbar's own menu doesn't open over it.
    root.Background(Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    auto right = [](Input::PointerRoutedEventArgs const& e) {
        auto props = e.GetCurrentPoint(nullptr).Properties();
        return props.IsRightButtonPressed() ||
               props.PointerUpdateKind() ==
                   winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased;
    };
    ui.pressed = root.PointerPressed(
        winrt::auto_revoke, [right](auto&&, Input::PointerRoutedEventArgs const& e) {
            core::Guard(L"Usage press", [&] {
                if (right(e)) {
                    e.Handled(true);
                }
            });
        });
    ui.released = root.PointerReleased(
        winrt::auto_revoke, [right](auto&&, Input::PointerRoutedEventArgs const& e) {
            core::Guard(L"Usage release", [&] {
                if (right(e)) {
                    e.Handled(true);
                }
            });
        });
    ui.rightTapped = root.RightTapped(
        winrt::auto_revoke, [](auto&&, Input::RightTappedRoutedEventArgs const& e) {
            core::Guard(L"Usage right-click", [&] {
                e.Handled(true);
                FlipLeft();
            });
        });
    ui.contextRequested = root.ContextRequested(
        winrt::auto_revoke, [](auto&&, Input::ContextRequestedEventArgs const& e) {
            core::Guard(L"Usage menu", [&] { e.Handled(true); });
        });
    ui.tapped = root.Tapped(
        winrt::auto_revoke, [p](auto&&, Input::TappedRoutedEventArgs const& e) {
            core::Guard(L"Usage click", [&] {
                e.Handled(true);
                if (p->root) {
                    usage_detail::Toggle(p->taskbar, p->root);
                }
            });
        });
    ui.root = root;
}

void SyncViews(Ui& ui, Model const& m) {
    UINT iconPx = IconSize(ui.taskbar);
    std::vector<Mark> marks;
    std::vector<std::wstring> keys;
    for (auto const& a : m.accounts) {
        marks.push_back(MarkOf(a, a.hasIcon && IconAt(a.icon, iconPx)));
        keys.push_back(a.kind + L"|" + std::to_wstring((int)marks.back()) + L"|" + a.icon);
    }
    bool same = ui.views.size() == m.accounts.size();
    for (size_t i = 0; same && i < ui.views.size(); i++) {
        same = ui.views[i].id == m.accounts[i].id && ui.views[i].mark == keys[i];
    }
    if (same) {
        return;
    }
    ui.panel.Children().Clear();
    ui.views.clear();
    for (size_t i = 0; i < m.accounts.size(); i++) {
        ui.views.push_back(MakeView(m.accounts[i].id, keys[i], marks[i]));
        ui.panel.Children().Append(ui.views.back().root);
    }
}

void RenderOn(HWND taskbar) {
    if (!g_model || g_model->accounts.empty()) {
        widgets::Remove(taskbar, kWidgetName);
        return;
    }
    auto it = std::ranges::find(g_uis, taskbar, &Ui::taskbar);
    auto& ui = it != g_uis.end() ? *it : g_uis.emplace_back();
    ui.taskbar = taskbar;
    EnsureRoot(ui);
    SyncViews(ui, *g_model);
    RefreshAll(ui);
    widgets::Add(taskbar, kWidgetName, kWidgetOrder, ui.root);
}

void Render() {
    std::erase_if(g_taskbars, [](HWND hwnd) { return !IsWindow(hwnd); });
    std::erase_if(g_uis, [](Ui const& ui) {
        return std::ranges::find(g_taskbars, ui.taskbar) == g_taskbars.end();
    });
    for (HWND taskbar : g_taskbars) {
        core::Guard(L"Usage widget", [&] { RenderOn(taskbar); });
    }
}

void OnModel(std::shared_ptr<const Model> model) {
    if (g_model && model && g_model->left != model->left) {
        g_leftFlip.reset();  // Bloom's own choice changed: it shows again
    }
    g_model = std::move(model);
    SyncIcons(g_model.get());
    Render();
    if (g_onModel) {
        core::Guard(L"Usage island", g_onModel);
    }
    core::Guard(L"Usage details", usage_detail::Refresh);
}

// ---- lifecycle -------------------------------------------------------------------------

// Also with a vertical taskbar, which Apply skips: the meter then shows once
// it turns horizontal.
void Start() {
    if (!InitPaths() || !IsDirectory(g_local)) {
        Wh_Log(L"No local app data folder; usage meter stays off");
        return;
    }
    g_pathsReady = true;
    g_stop = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_thread = g_stop ? CreateThread(nullptr, 0, WatchProc, nullptr, 0, nullptr)
                      : nullptr;
    if (!g_thread) {
        Wh_Log(L"Usage watcher did not start");
        if (g_stop) {
            CloseHandle(g_stop);
            g_stop = nullptr;
        }
    }
}

void Stop() {
    if (!g_thread) {
        return;
    }
    SetEvent(g_stop);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    CloseHandle(g_stop);
    g_thread = g_stop = nullptr;
}

void Apply(HWND hwnd) {
    if (core::g_vertical || core::g_unloading || !hwnd ||
        std::ranges::find(g_taskbars, hwnd) != g_taskbars.end()) {
        return;
    }
    g_taskbars.push_back(hwnd);
    // In an explorer that is starting up the watcher read the file before there
    // was a taskbar thread to hand it to; read it here once rather than wait for
    // Bloom's next write.
    if (!g_model && g_pathsReady) {
        std::string bytes;
        auto model = std::make_shared<Model>();
        if (ReadBytes(bytes) == Read::kOk && Parse(Widen(bytes), *model)) {
            OnModel(model);
            return;
        }
    }
    Render();
}

void Restore() {
    core::Guard(L"Usage details", usage_detail::Close);
    for (HWND taskbar : g_taskbars) {
        widgets::Remove(taskbar, kWidgetName);
    }
    g_taskbars.clear();
    g_uis.clear();
    g_icons.clear();
}

}  // namespace usage

// ============================================================================
namespace start {
// ============================================================================
// The Start button hidden, or turned into Bloom's: the button keeps its place,
// size, hover look and right-click menu, shows Bloom's flower instead of the
// Windows logo, and a click opens or closes Bloom instead of the Start menu.

namespace theme = settings::theme;

// Bloom's mark without its tile, in a 72 square (Bloom's tools\make_icon.ps1).
constexpr PCWSTR kPetals =
    L"M36,36.56C40.22,34.87 45.56,26.39 45.56,19.77"
    L"C45.56,14.26 42.7,10.4 40.22,10.68L36,14.11L31.78,10.68"
    L"C29.3,10.4 26.44,14.26 26.44,19.77C26.44,26.39 31.78,34.87 36,36.56Z"
    L"M37.34,37.53C40.25,41.02 49.97,43.48 56.26,41.44"
    L"C61.5,39.73 64.28,35.82 63.26,33.55L58.69,30.59L60.65,25.52"
    L"C60.14,23.08 55.59,21.55 50.35,23.25C44.06,25.29 37.64,33 37.34,37.53Z"
    L"M36.83,39.1C34.41,42.95 35.07,52.95 38.96,58.31"
    L"C42.2,62.76 46.78,64.2 48.63,62.52L50.02,57.26L55.45,57.56"
    L"C57.62,56.33 57.67,51.52 54.43,47.07C50.54,41.71 41.23,37.99 36.83,39.1Z"
    L"M35.17,39.1C30.77,37.99 21.46,41.71 17.57,47.07"
    L"C14.33,51.52 14.38,56.33 16.55,57.56L21.98,57.26L23.37,62.52"
    L"C25.22,64.2 29.8,62.76 33.04,58.31C36.93,52.95 37.59,42.95 35.17,39.1Z"
    L"M34.66,37.53C34.36,33 27.94,25.29 21.65,23.25"
    L"C16.41,21.55 11.86,23.08 11.35,25.52L13.31,30.59L8.74,33.55"
    L"C7.72,35.82 10.5,39.73 15.74,41.44C22.03,43.48 31.75,41.02 34.66,37.53Z";
constexpr PCWSTR kShowEvent = L"Local\\Bloom.Show";

// One per Start button that has the flower.
struct Flower {
    winrt::weak_ref<FrameworkElement> button;
    winrt::weak_ref<FrameworkElement> root;  // the button's template root
    Controls::Grid host{nullptr};            // the stock icon's
    Controls::Viewbox icon{nullptr};
    Shapes::Path petals{nullptr};
    Shapes::Ellipse rose{nullptr};
    FrameworkElement::SizeChanged_revoker stockSized;
    FrameworkElement::ActualThemeChanged_revoker themeChanged;
    UIElement::PointerPressed_revoker pressed;
    UIElement::PointerReleased_revoker released;
    UIElement::Tapped_revoker tapped;
};

[[clang::no_destroy]] std::list<Flower> g_flowers;

// Whether Bloom's launcher was open at the press: the press brings the taskbar
// forward and Bloom then hides the launcher, so a click that meant to close it
// must not open it again. At the press the taskbar is already in front and the
// launcher, about to hide, is still visible; it is asked then and only then.
bool g_openAtPress = false;
ULONGLONG g_pressedAt = 0;

// A launcher Bloom pinned stays shown, behind, when the press takes the focus
// from it, and is brought forward; one not pinned has hidden itself by this
// long after the press.
constexpr UINT kPinnedLookMs = 150;
UINT_PTR g_pinnedTimer = 0;

bool IsStartButton(FrameworkElement const& e) {
    return e && core::IsClass(e, L"Taskbar.ExperienceToggleButton") &&
           Automation::AutomationProperties::GetAutomationId(e) == L"StartButton";
}

FrameworkElement StartButtonOf(HWND taskbar) {
    auto root = core::TaskbarRoot(taskbar);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    return core::FindDescendant(content, IsStartButton, 10);
}

// Where Bloom runs from: its sign-in entry, else where its setup puts it.
std::wstring BloomExe() {
    WCHAR command[MAX_PATH * 2];
    DWORD size = sizeof(command);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                     L"Bloom", RRF_RT_REG_SZ, nullptr, command,
                     &size) == ERROR_SUCCESS &&
        command[0]) {
        std::wstring_view c = command;
        std::wstring exe(c[0] == L'"' ? c.substr(1, c.find(L'"', 1) - 1)
                                      : c.substr(0, c.find(L' ')));
        if (PathFileExistsW(exe.c_str())) {
            return exe;
        }
    }
    PWSTR local = nullptr;
    std::wstring exe;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local))) {
        exe = std::wstring(local) + L"\\BloomLauncher\\current\\Bloom.exe";
    }
    CoTaskMemFree(local);
    return PathFileExistsW(exe.c_str()) ? exe : std::wstring();
}

// The window is Bloom's launcher: Bloom's process, the launcher's title.
bool IsLauncher(HWND hwnd) {
    DWORD pid = 0;
    // A window of explorer's own would answer the title on its own thread.
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &pid) ||
        pid == GetCurrentProcessId()) {
        return false;
    }
    WCHAR title[8];
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) != 5 ||
        wcscmp(title, L"Bloom") != 0) {
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    WCHAR image[MAX_PATH];
    DWORD length = ARRAYSIZE(image);
    bool bloom = QueryFullProcessImageNameW(process, 0, image, &length) &&
                 _wcsicmp(image, BloomExe().c_str()) == 0;
    CloseHandle(process);
    return bloom;
}

// The front window is Bloom's launcher.
bool LauncherAhead() {
    return IsLauncher(GetForegroundWindow());
}

// Bloom's launcher is shown, in front or not. Bloom's notice cards carry its
// title too, and stay shown; they never take the focus, the launcher does.
bool LauncherShown() {
    bool shown = false;
    EnumWindows(
        [](HWND hwnd, LPARAM found) -> BOOL {
            if (IsWindowVisible(hwnd) &&
                !(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE) &&
                IsLauncher(hwnd)) {
                *reinterpret_cast<bool*>(found) = true;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&shown));
    return shown;
}

// A running Bloom waits for this event and then opens its launcher, or hides
// it when it's in front; otherwise Bloom starts with its launcher open.
void Show() {
    AllowSetForegroundWindow(ASFW_ANY);
    if (HANDLE event = OpenEventW(EVENT_MODIFY_STATE, FALSE, kShowEvent)) {
        SetEvent(event);
        CloseHandle(event);
        return;
    }
    std::wstring exe = BloomExe();
    if (exe.empty()) {
        Wh_Log(L"Bloom.exe not found");
        return;
    }
    std::wstring command = L"\"" + exe + L"\" --show";
    std::wstring dir = exe.substr(0, exe.find_last_of(L'\\'));
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(exe.c_str(), command.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, dir.c_str(), &si, &pi)) {
        Wh_Log(L"Bloom did not start: %u", GetLastError());
        return;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

void CALLBACK PinnedLook(HWND, UINT, UINT_PTR, DWORD) {
    KillTimer(nullptr, std::exchange(g_pinnedTimer, 0));
    core::Guard(L"Bloom pinned", [] {
        if (LauncherShown() && !LauncherAhead()) {
            Show();
        }
    });
}

void Paint(Flower& f) {
    auto root = f.root.get();
    bool dark = !root || root.ActualTheme() == ElementTheme::Dark;
    f.petals.Fill(Media::SolidColorBrush(dark ? theme::kUsageDark.text
                                              : theme::kUsageLight.text));
    f.rose.Fill(Media::SolidColorBrush(dark ? theme::kBloomRoseDark
                                            : theme::kBloomRoseLight));
}

void SizeIcon(Flower& f, double size) {
    size = size > 0 ? size : theme::kBloomIconFallback;
    f.icon.Width(size);
    f.icon.Height(size);
}

void Detach(Flower& f) {
    uint32_t index;
    if (f.host && f.icon && f.host.Children().IndexOf(f.icon, index)) {
        f.host.Children().RemoveAt(index);
    }
}

// The stock icon goes see-through, the flower sits in its cell at its size,
// and the left button's press and tap stop at the template root, so the
// button never sees them.
void Attach(FrameworkElement const& button) {
    auto root = core::ChildNamed(button, L"ExperienceToggleButtonRootPanel");
    auto stock = core::FindDescendant(
        root,
        [](FrameworkElement const& e) {
            return e.Name() == L"Icon" &&
                   std::wstring_view(winrt::get_class_name(e))
                           .find(L"AnimatedVisualPlayer") != std::wstring_view::npos;
        },
        6);
    auto host = stock ? core::Parent(stock).try_as<Controls::Grid>() : nullptr;
    if (!host) {
        Wh_Log(L"Start button: no stock icon in a grid");
        return;
    }
    std::erase_if(g_flowers, [](Flower const& f) { return !f.button.get(); });
    auto it = std::ranges::find_if(
        g_flowers, [&](Flower const& f) { return f.button.get() == button; });
    if (it != g_flowers.end()) {
        if (it->root.get() == root && it->host == host) {
            return;
        }
        Detach(*it);  // a new template: the old parts are on their way out
        g_flowers.erase(it);
    }

    std::wstring xaml =
        L"<Viewbox xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
        L"presentation\" IsHitTestVisible=\"False\"><Canvas Width=\"72\" "
        L"Height=\"72\"><Path Data=\"" +
        std::wstring(kPetals) +
        L"\"/><Ellipse Canvas.Left=\"31.78\" Canvas.Top=\"33.75\" "
        L"Width=\"8.44\" Height=\"8.44\"/></Canvas></Viewbox>";
    auto& f = g_flowers.emplace_back();
    Flower* p = &f;
    // A throw on the way takes the entry out again, so the next pass builds
    // the flower anew instead of finding it done.
    struct Unlist {
        std::list<Flower>::iterator it;
        bool done = false;
        ~Unlist() {
            if (!done) {
                core::Guard(L"Bloom icon", [&] { Detach(*it); });
                g_flowers.erase(it);
            }
        }
    } unlist{std::prev(g_flowers.end())};
    f.button = winrt::make_weak(button);
    f.root = winrt::make_weak(root);
    f.host = host;
    f.icon = Markup::XamlReader::Load(xaml).as<Controls::Viewbox>();
    auto shapes = f.icon.Child().as<Controls::Canvas>().Children();
    f.petals = shapes.GetAt(0).as<Shapes::Path>();
    f.rose = shapes.GetAt(1).as<Shapes::Ellipse>();
    Controls::Grid::SetRow(f.icon, Controls::Grid::GetRow(stock));
    Controls::Grid::SetColumn(f.icon, Controls::Grid::GetColumn(stock));
    Controls::Grid::SetRowSpan(f.icon, Controls::Grid::GetRowSpan(stock));
    Controls::Grid::SetColumnSpan(f.icon, Controls::Grid::GetColumnSpan(stock));
    f.icon.HorizontalAlignment(stock.HorizontalAlignment());
    f.icon.VerticalAlignment(stock.VerticalAlignment());
    f.icon.Margin(stock.Margin());
    SizeIcon(f, stock.ActualWidth() > 0 ? stock.ActualWidth() : stock.Width());
    Paint(f);
    host.Children().Append(f.icon);
    core::Keep(stock, UIElement::OpacityProperty(), winrt::box_value(0.0));
    // The button's MeasureOverride reads its tooltip as a ToolTip, so only the
    // words in it change; a plain string there takes explorer down.
    if (auto tip = Controls::ToolTipService::GetToolTip(button)
                       .try_as<Controls::ToolTip>()) {
        core::Set(tip, Controls::ContentControl::ContentProperty(),
                  winrt::box_value(winrt::hstring(L"Bloom")));
    }

    f.stockSized = stock.SizeChanged(
        winrt::auto_revoke, [p](auto&&, SizeChangedEventArgs const& e) {
            core::Guard(L"Bloom icon size", [&] { SizeIcon(*p, e.NewSize().Width); });
        });
    f.themeChanged = root.ActualThemeChanged(
        winrt::auto_revoke, [p](auto&&, auto&&) {
            core::Guard(L"Bloom icon theme", [p] { Paint(*p); });
        });
    f.pressed = root.PointerPressed(
        winrt::auto_revoke, [](auto&&, Input::PointerRoutedEventArgs const& e) {
            core::Guard(L"Bloom press", [&] {
                if (e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed()) {
                    e.Handled(true);
                    g_openAtPress = LauncherAhead() || LauncherShown();
                    g_pressedAt = GetTickCount64();
                    core::DiagLine(L"Bloom flower press, open %d", g_openAtPress);
                }
            });
        });
    f.released = root.PointerReleased(
        winrt::auto_revoke, [](auto&&, Input::PointerRoutedEventArgs const& e) {
            core::Guard(L"Bloom release", [&] {
                if (e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind() ==
                    winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased) {
                    e.Handled(true);
                }
            });
        });
    f.tapped = root.Tapped(
        winrt::auto_revoke, [](auto&&, Input::TappedRoutedEventArgs const& e) {
            core::Guard(L"Bloom click", [&] {
                e.Handled(true);
                bool open = g_openAtPress;
                g_openAtPress = false;
                // still in front: Bloom hides it itself on the signal
                if (!open || LauncherAhead()) {
                    Show();
                } else if (!g_pinnedTimer) {
                    ULONGLONG since = GetTickCount64() - g_pressedAt;
                    g_pinnedTimer = SetTimer(
                        nullptr, 0,
                        since < kPinnedLookMs ? UINT(kPinnedLookMs - since)
                                              : USER_TIMER_MINIMUM,
                        PinnedLook);
                }
            });
        });
    unlist.done = true;
    core::DiagLine(L"Start flower: stock %.1fx%.1f, host %s", stock.ActualWidth(),
                   stock.ActualHeight(), winrt::get_class_name(host).c_str());
}

// Width 0 takes its place away; the layout keeps setting its visibility, so
// that one stays the layout's.
void Hide(FrameworkElement const& button) {
    if (button.Width() == 0) {
        return;
    }
    auto parent = core::Parent(button);
    core::DiagLine(L"Start hidden: was %.1f wide, margin %.1f,%.1f, parent %s",
                   button.ActualWidth(), button.Margin().Left, button.Margin().Right,
                   parent ? winrt::get_class_name(parent).c_str() : L"-");
    core::Set(button, FrameworkElement::WidthProperty(), winrt::box_value(0.0));
    core::Set(button, UIElement::OpacityProperty(), winrt::box_value(0.0));
    core::Set(button, UIElement::IsHitTestVisibleProperty(), winrt::box_value(false));
}

void OnButton(FrameworkElement const& button) {
    if (!IsStartButton(button)) {
        return;
    }
    if (settings::g.start.mode == settings::StartMode::kHidden) {
        Hide(button);
    } else {
        Attach(button);
    }
}

void Apply(HWND hwnd) {
    if (core::g_vertical || core::g_unloading || !hwnd) {
        return;
    }
    size::g_onToggleButton = OnButton;
    if (auto button = StartButtonOf(hwnd)) {
        OnButton(button);
    } else {
        Wh_Log(L"Start button not found");
    }
}

void Restore() {
    size::g_onToggleButton = nullptr;
    for (auto& f : g_flowers) {
        core::Guard(L"Bloom icon", [&] { Detach(f); });
    }
    g_flowers.clear();
    g_openAtPress = false;
    if (g_pinnedTimer) {
        KillTimer(nullptr, std::exchange(g_pinnedTimer, 0));
    }
}

}  // namespace start

// ============================================================================
namespace island {
// ============================================================================
// F8: while an auto-hidden taskbar is in, a small black island sits on its
// edge of its monitor (top or bottom, where the taskbar is; in the middle or
// where it was dragged to), and the pointer entering it brings the taskbar
// out. As the taskbar comes out the island grows into the pill's shape and
// fades onto it; as the taskbar goes in, it shrinks back from the pill. Every
// taskbar has an island of its own (F11). An island is a popup window drawn
// with Windows.UI.Composition: a shape visual for the black body and its
// fillets into the edge, which the morph animates on the GPU, and a Direct2D
// surface for the content (clock, media, usage), redrawn only when what it
// shows changes. The taskbar's real expanded state decides each morph.
// Taskbar thread only, but for Stop.

#undef INTERFACE
#define INTERFACE ICompositorDesktopInterop
DECLARE_INTERFACE_IID_(ICompositorDesktopInterop,
                       IUnknown,
                       "29E691FA-4567-4DCA-B319-D0F207EB6807") {
    STDMETHOD(CreateDesktopWindowTarget)(HWND hwnd, BOOL isTopmost,
                                         void** result) PURE;
    STDMETHOD(EnsureOnThread)(DWORD threadId) PURE;
};

#undef INTERFACE
#define INTERFACE ICompositorInterop
DECLARE_INTERFACE_IID_(ICompositorInterop,
                       IUnknown,
                       "25297D5C-3AD4-4C9C-B5CF-E36A38512330") {
    STDMETHOD(CreateCompositionSurfaceForHandle)(HANDLE swapChain,
                                                 void** result) PURE;
    STDMETHOD(CreateCompositionSurfaceForSwapChain)(IUnknown * swapChain,
                                                    void** result) PURE;
    STDMETHOD(CreateGraphicsDevice)(IUnknown * renderingDevice,
                                    void** result) PURE;
};

#undef INTERFACE
#define INTERFACE ICompositionGraphicsDeviceInterop
DECLARE_INTERFACE_IID_(ICompositionGraphicsDeviceInterop,
                       IUnknown,
                       "A116FF71-F8BF-4C8A-9C98-70779A32A9C8") {
    STDMETHOD(GetRenderingDevice)(IUnknown * *value) PURE;
    STDMETHOD(SetRenderingDevice)(IUnknown * value) PURE;
};

#undef INTERFACE
#define INTERFACE ICompositionDrawingSurfaceInterop
DECLARE_INTERFACE_IID_(ICompositionDrawingSurfaceInterop,
                       IUnknown,
                       "FD04E6E3-FE0C-4C3C-AB19-A07601A576EE") {
    STDMETHOD(BeginDraw)(const RECT* updateRect, REFIID iid,
                         void** updateObject, POINT* updateOffset) PURE;
    STDMETHOD(EndDraw)() PURE;
    STDMETHOD(Resize)(SIZE sizePixels) PURE;
    STDMETHOD(Scroll)(const RECT* scrollRect, const RECT* clipRect,
                      int offsetX, int offsetY) PURE;
    STDMETHOD(ResumeDraw)() PURE;
    STDMETHOD(SuspendDraw)() PURE;
};

}  // namespace island

template <>
inline constexpr winrt::guid winrt::impl::guid_v<island::ICompositorDesktopInterop>{
    0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};
template <>
inline constexpr winrt::guid winrt::impl::guid_v<island::ICompositorInterop>{
    0x25297D5C, 0x3AD4, 0x4C9C, {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<island::ICompositionGraphicsDeviceInterop>{
        0xA116FF71, 0xF8BF, 0x4C8A, {0x9C, 0x98, 0x70, 0x77, 0x9A, 0x32, 0xA9, 0xC8}};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<island::ICompositionDrawingSurfaceInterop>{
        0xFD04E6E3, 0xFE0C, 0x4C3C, {0xAB, 0x19, 0xA0, 0x76, 0x01, 0xA5, 0x76, 0xEE}};

namespace island {

namespace theme = settings::theme;
namespace wuc = winrt::Windows::UI::Composition;
namespace wgd = winrt::Windows::Graphics::DirectX;
namespace wss = winrt::Windows::Storage::Streams;
using winrt::check_hresult;
using winrt::com_ptr;
using Color = winrt::Windows::UI::Color;

constexpr PCWSTR kClassName = L"PillTaskbarIsland_" WH_MOD_ID;
// From Stop: the windows go on their own thread.
constexpr UINT kStopMsg = WM_APP + 1;
// A pointer that comes onto an island from past its edge, where another
// monitor lies, is most likely on its way through: it brings the taskbar out
// only if it stays on the island this long.
constexpr UINT_PTR kCrossingTimer = 1;
constexpr UINT kCrossingMs = 120;
// Album art is kept at most this big; the island never draws it larger.
constexpr UINT kArtSize = 256;
constexpr float kPlaceholderAlpha = 0.25f;

UINT ShellHookMsg() {
    static UINT msg = RegisterWindowMessage(L"SHELLHOOK");
    return msg;
}

HINSTANCE Module() {
    static int anchor;
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (PCWSTR)&anchor, &module);
    return module;
}

// ---- shape ----------------------------------------------------------------------
// In window pixels, with y counted from the screen edge (a bottom island turns
// it over). The body hangs from the edge at y = 0 and has round corners of
// `radius` away from it; fillets of `flare` curve its sides out into the edge.

struct Shape {
    float width = 0;  // fillets included
    float height = 0;
    float flare = 0;
    float radius = 0;
    // A corner island's side at the screen's side: straight and square, with
    // no fillet or round corner there.
    bool flushLeft = false, flushRight = false;
    bool operator==(Shape const&) const = default;
};

// The left edge at `y`. It only moves right going down, so a pixel row's
// leftmost covered point is at the row's top.
float LeftEdge(Shape const& s, float y) {
    if (y < s.flare) {
        float d = s.flare - y;
        return std::sqrt(std::max(0.0f, s.flare * s.flare - d * d));
    }
    float corner = s.height - s.radius;
    if (y <= corner) {
        return s.flare;
    }
    float d = y - corner;
    return s.flare + s.radius -
           std::sqrt(std::max(0.0f, s.radius * s.radius - d * d));
}

// The edge's distance of a pixel row's point nearest it: the row itself at the
// top, the row's far side turned over at the bottom.
float FromEdge(Shape const& s, int row, bool bottom) {
    return (float)(bottom ? (int)s.height - 1 - row : row);
}

// True if the point, relative to the shape's top left, is in a pixel the
// shape touches.
// The pixel row's covered span [left, right).
std::pair<int, int> RowSpan(Shape const& s, int y, bool bottom) {
    int in = (int)std::floor(LeftEdge(s, FromEdge(s, y, bottom)));
    return {s.flushLeft ? 0 : in, (int)s.width - (s.flushRight ? 0 : in)};
}

bool InShape(Shape const& s, int x, int y, bool bottom) {
    if (y < 0 || y >= (int)s.height) {
        return false;
    }
    auto [left, right] = RowSpan(s, y, bottom);
    return x >= left && x < right;
}

// Every pixel the shape touches: the window region (hit-testing, and DWM's
// clip) must not cut into the antialiased edge.
HRGN Region(Shape const& s, bool bottom) {
    HRGN rgn = CreateRectRgn(0, 0, 0, 0);
    for (int y = 0; rgn && y < (int)s.height; y++) {
        auto [left, right] = RowSpan(s, y, bottom);
        if (HRGN row = CreateRectRgn(left, y, right, y + 1)) {
            CombineRgn(rgn, rgn, row, RGN_OR);
            DeleteObject(row);
        }
    }
    return rgn;
}

// A fillet in its `size` square: the square less the disc around its corner
// away from the body and from the edge. The left one meets the body at its
// right side; a bottom one is turned over.
com_ptr<ID2D1PathGeometry> Fillet(ID2D1Factory* factory, float size, bool right, bool bottom) {
    com_ptr<ID2D1PathGeometry> path;
    check_hresult(factory->CreatePathGeometry(path.put()));
    com_ptr<ID2D1GeometrySink> sink;
    check_hresult(path->Open(sink.put()));
    float f = size;
    auto at = [&](float x, float y) { return D2D1::Point2F(x, bottom ? f - y : y); };
    D2D1_POINT_2F start = right ? at(f, 0) : at(0, 0);
    sink->BeginFigure(start, D2D1_FIGURE_BEGIN_FILLED);
    if (right) {
        sink->AddLine(at(0, 0));
        sink->AddLine(at(0, f));
    } else {
        sink->AddLine(at(f, 0));
        sink->AddLine(at(f, f));
    }
    D2D1_ARC_SEGMENT arc = D2D1::ArcSegment(
        start, D2D1::SizeF(f, f), 0,
        right != bottom ? D2D1_SWEEP_DIRECTION_CLOCKWISE
                        : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
        D2D1_ARC_SIZE_SMALL);
    sink->AddArc(&arc);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    check_hresult(sink->Close());
    return path;
}

// ---- state ----------------------------------------------------------------------

struct Text {
    com_ptr<IDWriteTextLayout> layout;
    float width = 0;
    float height = 0;
};

// An account of the usage meter: its icon or letter, its tag and its number.
struct Meter {
    // The account's own icon, colored; or its letter (in `initial`), on a tile
    // when the icon won't draw.
    std::shared_ptr<const usage::IconPixels> icon;
    bool tile = false;
    float logoSize = 0;
    Text initial, tag, percent, unit;
    Color color{};  // the number's
    bool dim = false;
    float width = 0;
};

// What the island shows, measured.
struct Content {
    Text clock;
    bool media = false;
    float art = 0;  // diameter
    Text title;
    std::vector<Meter> meters;
    float usageLead = 0;  // before the meters, past the gap
    // Memory and CPU use as the memory add-on's taskbar icons, `unit` pixels a
    // unit of their 25 x 18 box.
    std::vector<std::pair<int, int>> system;  // kind (0 memory, 1 CPU), percent
    int systemStyle = 1;
    float unit = 0;
    float width = 0;
    std::wstring key;  // all that's drawn, to skip redraws that change nothing
};

enum class Phase {
    kIdle,        // the taskbar is in; the island shows
    kExpanding,   // growing into the pill as the taskbar comes out
    kOut,         // the taskbar is out; the island is invisible
    kCollapsing,  // shrinking back from the pill as the taskbar goes in
};

// What the window region was made from.
struct RegionKey {
    Phase phase = Phase::kIdle;
    RECT window{}, island{}, pill{}, sides[2]{};
    bool operator==(RegionKey const& o) const {
        return phase == o.phase && EqualRect(&window, &o.window) &&
               EqualRect(&island, &o.island) && EqualRect(&pill, &o.pill) &&
               EqualRect(&sides[0], &o.sides[0]) && EqualRect(&sides[1], &o.sides[1]);
    }
};

// An island against the left or right corner of the screen, in the same window
// as the middle one: its side there meets the screen's side square, the other
// curves into the edge. It doesn't grow into the pill; it fades as the middle
// one starts to.
struct Side {
    bool on = false;  // has something to show
    Shape shape;
    RECT rect{};  // screen pixels
    std::wstring drawn;
    wuc::ContainerVisual root{nullptr};
    wuc::CompositionRoundedRectangleGeometry body{nullptr};
    wuc::ShapeVisual bodyVisual{nullptr};
    wuc::SpriteVisual fillet{nullptr};
    wuc::SpriteVisual content{nullptr};
    wuc::CompositionDrawingSurface filletSurface{nullptr};
    wuc::CompositionDrawingSurface surface{nullptr};
    float filletSize = 0;
};

struct Island {
    int id = 0;  // for callbacks that may come after it's gone
    HWND hwnd = nullptr;
    HWND taskbar = nullptr;
    bool ready = false;
    UINT dpi = 96;
    bool shown = false;
    // The pointer is on the island; `reaching` if it came from outside, which
    // is what brings the taskbar out.
    bool inside = false;
    bool reaching = false;
    bool hover = false;       // faded for the pointer resting on it
    bool pressed = false;     // the left button went down on it
    bool dropTarget = false;  // its window is registered for drags
    bool fullscreen = false;  // a fullscreen app is active on the monitor
    HWND fullscreenWindow = nullptr;
    bool liftedTaskbar = false;  // made topmost to come out over a fullscreen window
    bool unreachable = false;  // logged that the taskbar can't be asked yet
    bool bottom = false;       // at the bottom edge, under a bottom taskbar
    bool sharedEdge = false;   // another monitor is past the island's edge
    // Moved along the edge with Ctrl held: `ctrl` as the last pointer move
    // had it, and while dragging where on the island the pointer holds it.
    bool ctrl = false;
    bool dragging = false;
    int grab = 0;
    // The monitor the island's place is kept for, as its HMONITOR last gave it.
    HMONITOR placeMonitor = nullptr;
    std::wstring placeId, placeName;
    Phase phase = Phase::kIdle;
    int morph = 0;  // counts morphs; only the last one's end counts
    // The clock's font for the clock and the numbers, the system UI font for
    // titles, and the usage's, as the widgets have them.
    std::wstring numberFont, textFont, usageFont;

    com_ptr<ID2D1Factory1> d2d;
    com_ptr<ID2D1Device> device;
    com_ptr<IDWriteFactory> dwrite;
    com_ptr<IWICImagingFactory> wic;

    wuc::Compositor compositor{nullptr};
    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::ContainerVisual shapes{nullptr};  // the body and fillets, faded together
    wuc::ShapeVisual bodyVisual{nullptr};
    wuc::CompositionRoundedRectangleGeometry body{nullptr};
    wuc::CompositionColorBrush fill{nullptr};  // the body's and the fillets'
    // The morph's progress (0 the island, 1 the pill) and both its ends.
    wuc::CompositionPropertySet morphState{nullptr};
    wuc::SpriteVisual fillets[2]{nullptr, nullptr};
    wuc::SpriteVisual content{nullptr};
    wuc::CompositionGraphicsDevice graphics{nullptr};
    wuc::CompositionDrawingSurface filletSurfaces[2]{nullptr, nullptr};
    wuc::CompositionDrawingSurface surface{nullptr};  // the content
    wuc::CompositionScopedBatch batch{nullptr};       // the running morph
    winrt::event_token batchDone{};
    // The taskbar's own content, as a composition visual of its XAML, and on
    // that compositor the morph's progress again, which its opacity follows.
    wuc::Visual bar{nullptr};
    wuc::CompositionPropertySet reveal{nullptr};

    Shape shape;
    float filletSize = 0;  // drawn in filletSurfaces
    std::wstring drawn;    // Content::key of the surface
    // Screen pixels: the window (the island and the pill together), the
    // island, and the taskbar's region box with its pill's corner radius,
    // last seen while the taskbar was out.
    RECT window{}, islandRect{}, pillBox{};
    float pillRadius = 0;
    bool pillSeen = false;
    RegionKey region;
    // The middle island has something to show. Without, it is still laid out,
    // the morph's start, but not drawn, hit or revealing.
    bool centerOn = true;
    Side sides[2];  // the left and the right corner's

    bool mediaVisible = false;
    std::wstring title;
    com_ptr<IWICBitmap> art;
    UINT artWidth = 0, artHeight = 0;
    int artId = 0;
};

// One per taskbar. A list: an island stays in place as others come and go.
[[clang::no_destroy]] std::list<Island> g_islands;
int g_lastId;
DWORD g_thread;              // the islands', the taskbar thread
std::atomic<HWND> g_window;  // any island's, for Stop
// Every island shows the primary taskbar's clock: a secondary one may have
// none.
[[clang::no_destroy]] winrt::weak_ref<Controls::TextBlock> g_clockText;
[[clang::no_destroy]] std::wstring g_clock;

void Sync(Island& g);

template <class F>
Island* FindIf(F&& match) {
    for (auto& g : g_islands) {
        if (g.ready && match(g)) {
            return &g;
        }
    }
    return nullptr;
}

Island* Of(HWND taskbar) {
    return FindIf([&](Island const& g) { return g.taskbar == taskbar; });
}

// The auto-hide hooks may ask from other threads; the list is this thread's.
bool OnIslandThread() {
    return g_thread && GetCurrentThreadId() == g_thread;
}

void SyncAll() {
    for (auto& g : g_islands) {
        core::Guard(L"Island sync", [&] { Sync(g); });
    }
}

// ---- system ---------------------------------------------------------------------

bool MonitorRect(Island const& g, RECT& out) {
    MONITORINFO info{.cbSize = sizeof(info)};
    if (!GetMonitorInfo(core::MonitorOf(g.taskbar), &info)) {
        return false;
    }
    out = info.rcMonitor;
    return true;
}

// Hidden or not, a top taskbar's window sits in the upper half, a bottom one's
// in the lower.
bool AtBottom(HWND taskbar, RECT const& monitor) {
    RECT rc;
    return GetWindowRect(taskbar, &rc) &&
           rc.top + rc.bottom >= monitor.top + monitor.bottom;
}

// ---- place along the edge ----------------------------------------------------------
// Where along its edge each monitor's island sits, from 0 (the left end) through
// 500 (the middle) to 1000 (the right end), kept in prefs.json in the add-on's
// data folder as "islandPositions": {"<monitor>": {"name": ..., "x": ...}}, where
// Bloom shows and changes it. Other fields of the file stay as they are. Read on
// the taskbar thread; the file is watched for changes made elsewhere.

constexpr int kPlaceMiddle = 500;
constexpr int kPlaceEnd = 1000;
constexpr PCWSTR kPrefsFile = L"prefs.json";
constexpr PCWSTR kPlacesField = L"islandPositions";
constexpr DWORD kMaxPrefsBytes = 256 * 1024;

// Monitor id → place, in thousandths; a place being dragged keeps its fraction.
[[clang::no_destroy]] std::unordered_map<std::wstring, double> g_places;
[[clang::no_destroy]] std::wstring g_prefsDir;  // empty without Bloom
FILETIME g_prefsWritten{};  // the file's time after this side's last read or write
HANDLE g_prefsChange = nullptr;
HANDLE g_prefsWait = nullptr;
bool g_placesRead = false;  // looked for the file since the islands started

namespace wdj = winrt::Windows::Data::Json;

// The add-on's data folder, made only while Bloom's own folder is there.
bool PrefsDir() {
    if (g_prefsDir.empty()) {
        WCHAR base[MAX_PATH];
        DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, ARRAYSIZE(base));
        if (!n || n >= ARRAYSIZE(base)) {
            return false;
        }
        std::wstring bloom = std::wstring(base) + L"\\" + usage::kBloomDirName;
        if (!usage::IsDirectory(bloom)) {
            return false;
        }
        CreateDirectoryW((bloom + L"\\addons").c_str(), nullptr);
        CreateDirectoryW((bloom + L"\\addons\\pill-taskbar").c_str(), nullptr);
        g_prefsDir = bloom + L"\\addons\\pill-taskbar";
    }
    return usage::IsDirectory(g_prefsDir);
}

std::wstring PrefsPath() {
    return g_prefsDir + L"\\" + kPrefsFile;
}

FILETIME PrefsTime() {
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(PrefsPath().c_str(), GetFileExInfoStandard, &data)) {
        return {};
    }
    return data.ftLastWriteTime;
}

// The file as an object; empty if it's missing or not JSON.
wdj::JsonObject ReadPrefs() {
    wdj::JsonObject root;
    winrt::file_handle file{CreateFileW(PrefsPath().c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    LARGE_INTEGER size{};
    if (!file || !GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > kMaxPrefsBytes) {
        return root;
    }
    std::string bytes((size_t)size.QuadPart, '\0');
    DWORD got = 0;
    if (!ReadFile(file.get(), bytes.data(), (DWORD)bytes.size(), &got, nullptr)) {
        return root;
    }
    bytes.resize(got);
    if (bytes.starts_with("\xEF\xBB\xBF")) {
        bytes.erase(0, 3);
    }
    wdj::JsonObject parsed{nullptr};
    if (wdj::JsonObject::TryParse(usage::Widen(bytes), parsed)) {
        root = parsed;
    }
    return root;
}

// Through a hidden temporary file, so Bloom never reads half of it.
bool WritePrefs(wdj::JsonObject const& root) {
    std::wstring text(root.Stringify());
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0,
                                nullptr, nullptr);
    std::string bytes(n > 0 ? n : 0, '\0');
    if (n > 0) {
        WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), bytes.data(), n,
                            nullptr, nullptr);
    }
    std::wstring path = PrefsPath(), temp = g_prefsDir + L"\\." + kPrefsFile + L".tmp";
    bool written = false;
    {
        winrt::file_handle file{CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                                            CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, nullptr)};
        DWORD put = 0;
        written = file && WriteFile(file.get(), bytes.data(), (DWORD)bytes.size(), &put,
                                    nullptr) &&
                  put == bytes.size();
    }
    if (written && MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING)) {
        g_prefsWritten = PrefsTime();
        return true;
    }
    DeleteFileW(temp.c_str());
    Wh_Log(L"Island place not saved: %u", GetLastError());
    return false;
}

void ReadPlaces() {
    g_prefsWritten = PrefsTime();
    g_places.clear();
    auto root = ReadPrefs();
    auto places = usage::Field(root, kPlacesField, wdj::JsonValueType::Object);
    if (!places) {
        return;
    }
    for (auto const& entry : places.GetObject()) {
        if (entry.Value().ValueType() != wdj::JsonValueType::Object) {
            continue;
        }
        double x = usage::Number(entry.Value().GetObject(), L"x", kPlaceMiddle);
        g_places[std::wstring(entry.Key())] = std::clamp(std::round(x), 0.0, (double)kPlaceEnd);
    }
}

// The island's place as it is now, in the file with its monitor's name; the
// rest of the file as it was.
void SavePlace(Island const& g) {
    if (!PrefsDir()) {
        return;
    }
    auto root = ReadPrefs();
    auto old = usage::Field(root, kPlacesField, wdj::JsonValueType::Object);
    wdj::JsonObject places = old ? old.GetObject() : wdj::JsonObject();
    auto it = g_places.find(g.placeId);
    double x = it != g_places.end() ? it->second : kPlaceMiddle;
    wdj::JsonObject record;
    record.Insert(L"name", wdj::JsonValue::CreateStringValue(g.placeName));
    record.Insert(L"x", wdj::JsonValue::CreateNumberValue(std::round(x)));
    places.Insert(g.placeId, record);
    root.Insert(kPlacesField, places);
    WritePrefs(root);
}

// Taskbar thread: the file changed, from Bloom or by hand, unless it was this
// side's own write. A drag under way keeps its island where the pointer has it.
void OnPrefsChanged() {
    FILETIME now = PrefsTime();
    if (CompareFileTime(&now, &g_prefsWritten) == 0) {
        return;
    }
    std::unordered_map<std::wstring, double> dragged;
    for (auto& g : g_islands) {
        if (g.dragging && g_places.contains(g.placeId)) {
            dragged[g.placeId] = g_places[g.placeId];
        }
    }
    ReadPlaces();
    for (auto& [id, x] : dragged) {
        g_places[id] = x;
    }
    SyncAll();
}

void CALLBACK OnPrefsFolderChanged(void*, BOOLEAN) {
    FindNextChangeNotification(g_prefsChange);
    core::Post(OnPrefsChanged);
}

// Once, as the first island needs a place: reads the file and starts watching
// its folder.
void WatchPlaces() {
    if (std::exchange(g_placesRead, true) || !PrefsDir()) {
        return;
    }
    ReadPlaces();
    g_prefsChange = FindFirstChangeNotificationW(g_prefsDir.c_str(), FALSE,
                                                 FILE_NOTIFY_CHANGE_LAST_WRITE |
                                                     FILE_NOTIFY_CHANGE_FILE_NAME);
    if (g_prefsChange == INVALID_HANDLE_VALUE) {
        g_prefsChange = nullptr;
        return;
    }
    if (!RegisterWaitForSingleObject(&g_prefsWait, g_prefsChange, OnPrefsFolderChanged,
                                     nullptr, INFINITE, WT_EXECUTEDEFAULT)) {
        g_prefsWait = nullptr;
        Wh_Log(L"Island places not watched: %u", GetLastError());
    }
}

// At restore: waits out a callback under way.
void UnwatchPlaces() {
    if (HANDLE wait = std::exchange(g_prefsWait, nullptr)) {
        UnregisterWaitEx(wait, INVALID_HANDLE_VALUE);
    }
    if (HANDLE change = std::exchange(g_prefsChange, nullptr)) {
        FindCloseChangeNotification(change);
    }
    g_places.clear();
    g_placesRead = false;
}

// A name for the monitor that stays the same across restarts: its display's
// hardware and instance from the device path ("DEL41D9_5_1a2b3c_0_UID4352"),
// else its display name. A label as Bloom shows it: the model and the size.
void NotePlaceMonitor(Island& g, HMONITOR monitor) {
    if (monitor == g.placeMonitor && !g.placeId.empty()) {
        return;
    }
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return;
    }
    std::wstring device = info.szDevice;
    std::wstring id = device.starts_with(L"\\\\.\\") ? device.substr(4) : device;
    std::wstring model = id;
    DISPLAY_DEVICEW dd{.cb = sizeof(dd)};
    if (EnumDisplayDevicesW(info.szDevice, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME)) {
        // \\?\DISPLAY#<hardware>#<instance>#{class}
        std::wstring path = dd.DeviceID;
        std::vector<std::wstring> parts;
        for (size_t start = 0, end; start <= path.size(); start = end + 1) {
            end = path.find(L'#', start);
            if (end == std::wstring::npos) {
                end = path.size();
            }
            parts.push_back(path.substr(start, end - start));
        }
        if (parts.size() >= 3 && !parts[1].empty()) {
            model = parts[1];
            id = parts[1] + L"_" + parts[2];
        }
    }
    for (wchar_t& c : id) {
        if (!iswalnum(c) && c != L'-') {
            c = L'_';
        }
    }
    RECT const& r = info.rcMonitor;
    g.placeMonitor = monitor;
    g.placeId = id;
    g.placeName = model + L" " + std::to_wstring(r.right - r.left) + L"\u00D7" +
                  std::to_wstring(r.bottom - r.top);
    // A monitor Bloom doesn't list yet is listed, in the middle, so its place
    // can be set there before it's ever dragged.
    if (g_prefsChange && !g_places.contains(id)) {
        g_places[id] = kPlaceMiddle;
        SavePlace(g);
    }
}

// The island's left on the monitor, for its width.
int PlacedLeft(Island const& g, RECT const& monitor, int width) {
    auto it = g_places.find(g.placeId);
    double x = it != g_places.end() ? it->second : kPlaceMiddle;
    int room = std::max(0, (int)(monitor.right - monitor.left) - width);
    return monitor.left + (int)std::lround(room * x / kPlaceEnd);
}

// The place for an island whose left is at `left`.
void SetPlace(Island const& g, RECT const& monitor, int left) {
    int width = g.islandRect.right - g.islandRect.left;
    int room = (int)(monitor.right - monitor.left) - width;
    g_places[g.placeId] =
        room > 0 ? std::clamp((double)(left - monitor.left) * kPlaceEnd / room, 0.0,
                              (double)kPlaceEnd)
                 : kPlaceMiddle;
}

// The first of `names` that is installed.
std::wstring PickFont(IDWriteFactory* dwrite, std::initializer_list<PCWSTR> names) {
    com_ptr<IDWriteFontCollection> fonts;
    if (SUCCEEDED(dwrite->GetSystemFontCollection(fonts.put(), FALSE))) {
        for (PCWSTR name : names) {
            UINT32 index = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(fonts->FindFamilyName(name, &index, &exists)) && exists) {
                return name;
            }
        }
    }
    return L"Segoe UI";
}

// The island's morph, and the taskbar's slide with it.
int MorphMs() {
    int ms = settings::g.autohide.animation;
    return ms > 0 ? ms : theme::kIslandMorphMs;
}

// The island's colors: black with white text (its default), or white with the
// light theme's.
bool DarkIsland() {
    return !settings::g.island.white;
}

Color IslandFill() {
    return DarkIsland() ? theme::kIslandFill : theme::kIslandWhiteFill;
}

Color IslandText(bool dim) {
    return DarkIsland() ? (dim ? theme::kIslandDimText : theme::kIslandText)
                        : (dim ? theme::kIslandWhiteDimText : theme::kIslandWhiteText);
}

// The glass pill's look at a glance: its tint, as strong as it's set; without
// glass the pill's opaque fill.
Color PillColor() {
    Color c = settings::Dark() ? theme::kIslandPillDark : theme::kIslandPillLight;
    c.A = settings::g.pill.glass ? (uint8_t)std::lround(settings::g.pill.tintOpacity * 2.55) : 255;
    return c;
}

// ---- memory and CPU, from the memory add-on --------------------------------------
// Memory Clean keeps its readings on the primary taskbar window and posts a note
// there as they change (docs/addon-protocol.md). Nothing is read here but that.

constexpr WCHAR kFiguresProp[] = L"MemoryClean.Figures";
constexpr int kMemory = 0, kCpu = 1;

struct Figures {
    bool known = false;  // the add-on runs
    int memory = 0, cpu = 0;
    int style = 1;  // its icon: 0 line, 1 fill, 2 solid
};

// Bits 0-7 memory use, 8-15 CPU use, 16-17 the icon style, 24 set while it runs.
Figures ReadFigures() {
    HWND primary = FindWindow(L"Shell_TrayWnd", nullptr);
    auto v = primary ? (ULONG_PTR)GetProp(primary, kFiguresProp) : 0;
    Figures f;
    f.known = (v >> 24) & 1;
    f.memory = std::min<int>(v & 0xFF, 100);
    f.cpu = std::min<int>((v >> 8) & 0xFF, 100);
    f.style = std::min<int>((v >> 16) & 3, 2);
    return f;
}

bool ShowsSystem() {
    auto const& set = settings::g.island;
    return set.showMemory || set.showCpu;
}

// The add-on's icons in their 25 x 18 box, as it draws them on the taskbar: a RAM
// module on six feet, the middle one left out for the key notch; a processor with
// pins on all four sides.
constexpr float kMeterW = 25, kMeterH = 18, kMeterStroke = 1.3f, kPinW = 1.7f;

struct Box4 {
    float x, y, w, h;
};
struct MeterOutline {
    Box4 body;
    float radius;
    std::vector<Box4> pins;
};

MeterOutline const& OutlineOf(int kind) {
    static const MeterOutline ram = [] {
        MeterOutline o{{1.15f, 0.95f, kMeterW - 2.3f, 13.3f}, 3.6f, {}};
        float top = o.body.y + o.body.h - kMeterStroke / 2;
        for (float x : {-7.0f, -4.4f, -1.8f, 1.8f, 4.4f, 7.0f}) {
            o.pins.push_back({kMeterW / 2 + x - kPinW / 2, top, kPinW, 2.9f + kMeterStroke / 2});
        }
        return o;
    }();
    // The processor's body is as tall as the module's and in the same place, so the numbers
    // of the two stand at one height; its side pins end where the module's body does and its
    // legs where the module's feet do.
    static const MeterOutline cpu = [] {
        Box4 b = ram.body;
        float reach = 1.8f;  // how far the side pins stand out of the body
        MeterOutline o{{b.x + reach, b.y, b.w - 2 * reach, b.h}, 3.2f, {}};
        float cx = o.body.x + o.body.w / 2, cy = o.body.y + o.body.h / 2;
        float right = o.body.x + o.body.w - kMeterStroke / 2;
        for (float y : {-2.8f, 2.8f}) {
            o.pins.push_back({o.body.x - reach, cy + y - kPinW / 2, reach + kMeterStroke / 2, kPinW});
            o.pins.push_back({right, cy + y - kPinW / 2, reach + kMeterStroke / 2, kPinW});
        }
        for (float x : {-5.4f, 0.0f, 5.4f}) {
            o.pins.push_back({cx + x - kPinW / 2, ram.pins[0].y, kPinW, ram.pins[0].h});
        }
        return o;
    }();
    return kind == kCpu ? cpu : ram;
}

// The add-on's colours (Bloom's tokens), the dark theme's on the black island and the
// light one's on white: its ink, amber from 80 %, red from 90 %.
Color SystemColor(int percent) {
    bool dark = DarkIsland();
    if (percent >= 90) {
        return dark ? Color{255, 0xE5, 0x48, 0x4D} : Color{255, 0xDC, 0x3E, 0x42};
    }
    if (percent >= 80) {
        return dark ? Color{255, 0xF7, 0xB7, 0x50} : Color{255, 0xB7, 0x79, 0x1F};
    }
    return dark ? Color{255, 0xEC, 0xED, 0xEE} : Color{255, 0x11, 0x18, 0x1C};
}

// ---- devices --------------------------------------------------------------------

void CreateDevice(Island& g) {
    com_ptr<ID3D11Device> d3d;
    HRESULT hr = E_FAIL;
    for (auto type : {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP}) {
        hr = D3D11CreateDevice(nullptr, type, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                               D3D11_SDK_VERSION, d3d.put(), nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            break;
        }
    }
    check_hresult(hr);
    com_ptr<ID2D1Device> device;
    check_hresult(g.d2d->CreateDevice(d3d.as<IDXGIDevice>().get(), device.put()));
    g.device = device;
}

bool DeviceLost(winrt::hresult hr) {
    return hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET ||
           hr == D2DERR_RECREATE_TARGET;
}

// The surfaces stay, and are drawn again after this.
void Recover(Island& g) {
    CreateDevice(g);
    check_hresult(g.graphics.as<ICompositionGraphicsDeviceInterop>()
                      ->SetRenderingDevice(g.device.get()));
}

// ---- drawing --------------------------------------------------------------------

template <class F>
void Paint(wuc::CompositionDrawingSurface const& surface, F&& draw) {
    auto interop = surface.as<ICompositionDrawingSurfaceInterop>();
    com_ptr<ID2D1DeviceContext> dc;
    POINT offset{};
    check_hresult(interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext),
                                     dc.put_void(), &offset));
    try {
        // Pixels, at the surface's place in its atlas.
        dc->SetDpi(96, 96);
        D2D1_MATRIX_3X2_F transform =
            D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y);
        dc->SetTransform(&transform);
        D2D1_COLOR_F clear{};
        dc->Clear(&clear);
        dc->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        draw(dc.get());
    } catch (...) {
        interop->EndDraw();
        throw;
    }
    check_hresult(interop->EndDraw());
}

com_ptr<ID2D1SolidColorBrush> Brush(ID2D1DeviceContext* dc,
                                    Color color,
                                    float opacity = 1) {
    D2D1_COLOR_F c{color.R / 255.0f, color.G / 255.0f, color.B / 255.0f,
                   color.A / 255.0f * opacity};
    com_ptr<ID2D1SolidColorBrush> brush;
    check_hresult(dc->CreateSolidColorBrush(&c, nullptr, brush.put()));
    return brush;
}

void DrawFillets(Island& g) {
    for (int i = 0; i < 2; i++) {
        auto fillet = Fillet(g.d2d.get(), g.filletSize, i == 1, g.bottom);
        Paint(g.filletSurfaces[i], [&](ID2D1DeviceContext* dc) {
            dc->FillGeometry(fillet.get(), Brush(dc, IslandFill()).get(), nullptr);
        });
    }
}

void DrawArt(Island& g,
             ID2D1DeviceContext* dc,
             float left,
             float top,
             float size,
             Color placeholder) {
    D2D1_ELLIPSE circle{{left + size / 2, top + size / 2}, size / 2, size / 2};
    com_ptr<ID2D1Bitmap1> bitmap;
    if (g.art && FAILED(dc->CreateBitmapFromWicBitmap(g.art.get(), bitmap.put()))) {
        bitmap = nullptr;
    }
    if (!bitmap || !g.artWidth || !g.artHeight) {
        dc->FillEllipse(&circle, Brush(dc, placeholder, kPlaceholderAlpha).get());
        return;
    }
    com_ptr<ID2D1EllipseGeometry> clip;
    check_hresult(g.d2d->CreateEllipseGeometry(&circle, clip.put()));
    // Covers the circle, as the widget's UniformToFill.
    float scale = std::max(size / g.artWidth, size / g.artHeight);
    float w = g.artWidth * scale, h = g.artHeight * scale;
    D2D1_RECT_F dest{left + (size - w) / 2, top + (size - h) / 2,
                     left + (size + w) / 2, top + (size + h) / 2};
    dc->PushLayer(D2D1::LayerParameters1(D2D1::InfiniteRect(), clip.get()), nullptr);
    dc->DrawBitmap(bitmap.get(), &dest, 1.0f,
                   D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC, nullptr, nullptr);
    dc->PopLayer();
}

// One of the memory add-on's icons as it draws it on the taskbar, its box's top
// left at (x, y), `u` pixels a unit of the box.
void DrawSystem(Island& g, ID2D1DeviceContext* dc, int kind, int percent, int style, float x,
                float y, float u) {
    MeterOutline const& o = OutlineOf(kind);
    Color color = SystemColor(percent);
    auto ink = Brush(dc, color);
    auto box = [&](Box4 const& b, float inset) {
        return D2D1::RectF(x + b.x * u + inset, y + b.y * u + inset, x + (b.x + b.w) * u - inset,
                           y + (b.y + b.h) * u - inset);
    };
    D2D1_RECT_F body = box(o.body, 0);
    float radius = o.radius * u;
    if (style != 0) {
        D2D1_RECT_F clip = body;
        if (style == 1) {
            clip.right = body.left + (body.right - body.left) * std::clamp(percent, 0, 100) / 100.0f;
        }
        dc->PushAxisAlignedClip(&clip, D2D1_ANTIALIAS_MODE_ALIASED);
        D2D1_ROUNDED_RECT fill{body, radius, radius};
        dc->FillRoundedRectangle(&fill, Brush(dc, color, style == 2 ? 1.0f : 0x47 / 255.0f).get());
        dc->PopAxisAlignedClip();
    }
    for (auto const& p : o.pins) {
        auto pin = box(p, 0);
        dc->FillRectangle(&pin, ink.get());
    }
    // Stroked inside its bounds, as XAML strokes a Rectangle.
    float stroke = kMeterStroke * u, inner = std::max(0.0f, radius - stroke / 2);
    D2D1_ROUNDED_RECT outline{box(o.body, stroke / 2), inner, inner};
    dc->DrawRoundedRectangle(&outline, ink.get(), stroke, nullptr);

    std::wstring number = std::to_wstring(percent);
    com_ptr<IDWriteTextFormat> format;
    com_ptr<IDWriteTextLayout> layout;
    DWRITE_OVERHANG_METRICS over{};
    if (FAILED(g.dwrite->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_BOLD,
                                          DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                          (percent >= 100 ? 7.8f : 10.0f) * u, L"",
                                          format.put())) ||
        FAILED(format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP)) ||
        FAILED(g.dwrite->CreateTextLayout(number.c_str(), (UINT32)number.size(), format.get(), 0,
                                          0, layout.put())) ||
        FAILED(layout->GetOverhangMetrics(&over))) {
        return;
    }
    // The ink, not the line box, in the middle of the body.
    float cx = (body.left + body.right) / 2, cy = (body.top + body.bottom) / 2;
    Color win = DarkIsland() ? Color{255, 0x18, 0x18, 0x1B} : Color{255, 0xF4, 0xF4, 0xF5};
    dc->DrawTextLayout({std::round(cx - (over.right - over.left) / 2),
                        std::round(cy - (over.bottom - over.top) / 2)},
                       layout.get(), Brush(dc, style == 2 ? win : color).get(),
                       D2D1_DRAW_TEXT_OPTIONS_NONE);
}

// What `c` holds, in the body of `shape` on `surface`: the middle island's or a
// corner one's.
void DrawContent(Island& g,
                 Content const& c,
                 wuc::CompositionDrawingSurface const& surface,
                 Shape const& shape) {
    Paint(surface, [&](ID2D1DeviceContext* dc) {
        float s = g.dpi / 96.0f;
        float h = shape.height;
        float gap = std::round((float)theme::kIslandGap * s);
        Color textColor = IslandText(false);
        Color dimColor = IslandText(true);
        auto text = Brush(dc, textColor);
        auto put = [&](Text const& t, float left, float top, ID2D1Brush* brush) {
            dc->DrawTextLayout({left, top}, t.layout.get(), brush,
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
        };
        // A set width narrower than the content cuts it at the body.
        D2D1_RECT_F bodyRect{shape.flushLeft ? 0 : shape.flare, 0,
                             shape.width - (shape.flushRight ? 0 : shape.flare), h};
        dc->PushAxisAlignedClip(&bodyRect, D2D1_ANTIALIAS_MODE_ALIASED);

        float x = std::round(bodyRect.left + (bodyRect.right - bodyRect.left - c.width) / 2);
        if (c.clock.layout) {
            put(c.clock, x, (h - c.clock.height) / 2, text.get());
            x += c.clock.width + gap;
        }
        if (c.media) {
            DrawArt(g, dc, x, (h - c.art) / 2, c.art, dimColor);
            x += c.art;
            if (c.title.layout) {
                x += std::round((float)theme::kIslandArtGap * s);
                put(c.title, x, (h - c.title.height) / 2, text.get());
                x += c.title.width;
            }
            x += gap;
        }
        // One line: the island is too low for the widget's bar.
        float logoGap = std::round((float)theme::kIslandMeterGap * s);
        x += c.usageLead;
        for (auto const& m : c.meters) {
            float opacity = m.dim ? (float)theme::kUsageDimOpacity : 1.0f;
            auto ink = Brush(dc, textColor, opacity);
            auto figure = Brush(dc, m.color, opacity);
            float left = x;
            float top = std::round((h - m.logoSize) / 2);
            if (m.icon) {
                UINT px = m.icon->size;
                com_ptr<ID2D1Bitmap1> bitmap;
                auto props = D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_NONE,
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                if (SUCCEEDED(dc->CreateBitmap({px, px}, m.icon->bgra.data(), px * 4, &props,
                                               bitmap.put()))) {
                    D2D1_RECT_F dest{x, top, x + px, top + px};
                    dc->DrawBitmap(bitmap.get(), &dest, opacity,
                                   D2D1_INTERPOLATION_MODE_LINEAR, nullptr, nullptr);
                }
                x += m.logoSize;
            } else if (m.tile) {
                float r = m.logoSize * (float)theme::kUsageTileRadius;
                D2D1_ROUNDED_RECT tile{{x, top, x + m.logoSize, top + m.logoSize}, r, r};
                Color fill = textColor;
                fill.A = theme::kUsageTileAlpha;
                dc->FillRoundedRectangle(&tile, Brush(dc, fill, opacity).get());
                put(m.initial, x + (m.logoSize - m.initial.width) / 2,
                    top + (m.logoSize - m.initial.height) / 2, ink.get());
                x += m.logoSize;
            } else {
                put(m.initial, x, (h - m.initial.height) / 2, ink.get());
                x += m.initial.width;
            }
            x += logoGap;
            if (m.tag.layout) {
                put(m.tag, x, (h - m.tag.height) / 2, ink.get());
                x += m.tag.width + logoGap;
            }
            put(m.percent, x, (h - m.percent.height) / 2, figure.get());
            if (m.unit.layout) {
                put(m.unit, x + m.percent.width, (h - m.unit.height) / 2, figure.get());
            }
            x = left + m.width + gap;
        }
        // Memory and CPU are two items, as far apart as any two.
        for (auto [kind, percent] : c.system) {
            DrawSystem(g, dc, kind, percent, c.systemStyle, x, std::round((h - kMeterH * c.unit) / 2),
                       c.unit);
            x += kMeterW * c.unit + gap;
        }
        dc->PopAxisAlignedClip();
    });
}

// ---- content --------------------------------------------------------------------

// `tabular`: the numbers' font with digits of one width, so the island keeps
// its size as the time moves; else the UI font, trimmed to `maxWidth` if set.
// `family` stands in for either font.
Text MakeText(Island& g,
              std::wstring const& text,
              float size,
              DWRITE_FONT_WEIGHT weight,
              float maxWidth,
              bool tabular,
              std::wstring const* family = nullptr) {
    com_ptr<IDWriteTextFormat> format;
    std::wstring const& font = family ? *family : tabular ? g.numberFont : g.textFont;
    check_hresult(g.dwrite->CreateTextFormat(
        font.c_str(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, size, L"", format.put()));
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    if (maxWidth > 0) {
        com_ptr<IDWriteInlineObject> ellipsis;
        check_hresult(g.dwrite->CreateEllipsisTrimmingSign(format.get(), ellipsis.put()));
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        format->SetTrimming(&trimming, ellipsis.get());
    }
    Text t;
    check_hresult(g.dwrite->CreateTextLayout(
        text.c_str(), (UINT32)text.size(), format.get(),
        maxWidth > 0 ? maxWidth : 4096.0f, size * 4, t.layout.put()));
    if (tabular) {
        com_ptr<IDWriteTypography> typography;
        if (SUCCEEDED(g.dwrite->CreateTypography(typography.put()))) {
            typography->AddFontFeature({DWRITE_FONT_FEATURE_TAG_TABULAR_FIGURES, 1});
            t.layout->SetTypography(typography.get(), {0, (UINT32)text.size()});
        }
    }
    DWRITE_TEXT_METRICS metrics{};
    check_hresult(t.layout->GetMetrics(&metrics));
    t.width = std::ceil(metrics.widthIncludingTrailingWhitespace);
    t.height = metrics.height;
    return t;
}

// The items set to the island at `slot`. `room`, if set, is the width the
// content must fit: the title gives way first, down to an ellipsis, then goes.
Content Measure(Island& g, float height, float room, settings::Slot slot) {
    auto const& set = settings::g.island;
    auto here = [&](bool show, settings::Slot at) { return show && at == slot; };
    float s = g.dpi / 96.0f;
    float size = std::round(height * (float)theme::kIslandTextSize);
    float gap = std::round((float)theme::kIslandGap * s);
    float artGap = std::round((float)theme::kIslandArtGap * s);
    constexpr WCHAR kSeparator = L'\x1F';
    Content c;
    auto add = [&](float width) { c.width += (c.width > 0 ? gap : 0) + width; };

    if (here(set.showClock, set.clockAt) && !g_clock.empty()) {
        c.clock = MakeText(g, g_clock, size, DWRITE_FONT_WEIGHT_NORMAL, 0, true);
        add(c.clock.width);
        c.key += g_clock;
    }
    c.key += kSeparator;
    if (here(set.showMedia, set.mediaAt) && g.mediaVisible) {
        c.media = true;
        c.art = std::round(height * (float)theme::kIslandArtSize);
        float width = c.art;
        if (!g.title.empty()) {
            c.title = MakeText(g, g.title, size, DWRITE_FONT_WEIGHT_NORMAL,
                               std::round(settings::g.media.maxWidth * s), false);
            width += artGap + c.title.width;
        }
        add(width);
        c.key += g.title + kSeparator + std::to_wstring(g.artId);
    }
    c.key += kSeparator;
    // Staleness is judged with the current time, as the widget does.
    auto model = usage::g_model;
    if (here(set.showUsage, set.usageAt) && model) {
        auto labels = usage::Labels(*model);
        int64_t now = usage::Now();
        float logoGap = std::round((float)theme::kIslandMeterGap * s);
        for (size_t i = 0; i < model->accounts.size(); i++) {
            auto const& a = model->accounts[i];
            auto value = usage::Evaluate(*model, a, usage::BarLimit(a), now);
            Meter m;
            // The dark theme's colors on the black island, the light one's on white.
            auto icon = a.hasIcon ? usage::IconAt(a.icon, (UINT)size) : nullptr;
            auto mark = usage::MarkOf(a, icon != nullptr);
            m.logoSize = size;
            if (mark == usage::Mark::kIcon) {
                bool mono = settings::g.usage.logoColor != settings::LogoColor::kBrand;
                m.icon = usage::Colored(icon, mono, usage::IconTint(mono, DarkIsland()));
            } else if (mark == usage::Mark::kTile) {
                m.tile = true;
                m.initial = MakeText(g, labels[i].letter,
                                     std::round(size * (float)theme::kUsageTileLetter),
                                     DWRITE_FONT_WEIGHT_SEMI_BOLD, 0, false, &g.usageFont);
            } else {
                m.initial = MakeText(g, labels[i].initial, std::round(size * 0.85f),
                                     DWRITE_FONT_WEIGHT_SEMI_BOLD, 0, true);
            }
            if (!labels[i].tag.empty()) {
                m.tag = MakeText(g, labels[i].tag, size, DWRITE_FONT_WEIGHT_NORMAL,
                                 std::round(size * (float)theme::kUsageTagMaxEm), false,
                                 &g.usageFont);
            }
            m.percent = MakeText(g, value.Figure(), size, DWRITE_FONT_WEIGHT_MEDIUM, 0, true,
                                 &g.usageFont);
            if (value.kind == usage::Value::kNumber) {
                m.unit = MakeText(g, theme::kUsageUnit, size, DWRITE_FONT_WEIGHT_NORMAL, 0,
                                  false, &g.usageFont);
            }
            m.color = usage::FigureColor(value, DarkIsland());
            m.dim = usage::Dim(*model, a, now);
            bool square = m.icon || m.tile;
            m.width = (square ? m.logoSize : m.initial.width) + logoGap +
                      (m.tag.layout ? m.tag.width + logoGap : 0) + m.percent.width +
                      m.unit.width;
            if (c.meters.empty() && c.width > 0) {
                c.usageLead = std::round((float)theme::kIslandUsageLead * s);
                c.width += c.usageLead;
            }
            add(m.width);
            // The color too: 79.6 and 80.2 both read "80".
            c.key += labels[i].initial + std::to_wstring((int)mark) + a.icon +
                     labels[i].tag + kSeparator + value.Text() +
                     std::to_wstring(m.color.R << 16 | m.color.G << 8 | m.color.B) +
                     (m.dim ? L"d" : L"") + kSeparator;
            c.meters.push_back(std::move(m));
        }
        c.key += std::to_wstring((int)settings::g.usage.logoColor) + kSeparator +
                 std::to_wstring(usage::g_iconsRead);
    }

    c.key += kSeparator;
    // Memory and CPU at the height the island gives its art, less a little.
    if (here(set.showMemory, set.memoryAt) || here(set.showCpu, set.cpuAt)) {
        Figures f = ReadFigures();
        if (f.known) {
            if (here(set.showMemory, set.memoryAt)) {
                c.system.push_back({kMemory, f.memory});
            }
            if (here(set.showCpu, set.cpuAt)) {
                c.system.push_back({kCpu, f.cpu});
            }
            c.systemStyle = f.style;
            c.unit = std::round(height * (float)theme::kIslandSystemSize) / kMeterH;
            for (auto [kind, percent] : c.system) {
                add(kMeterW * c.unit);
                c.key += std::to_wstring(kind) + L":" + std::to_wstring(percent) + L",";
            }
            c.key += std::to_wstring(f.style) + std::to_wstring((int)DarkIsland());
        }
    }

    if (room > 0 && c.width > room && c.title.layout) {
        float before = c.title.width;
        float titleWidth = before - (c.width - room);
        if (titleWidth >= size) {
            c.title = MakeText(g, g.title, size, DWRITE_FONT_WEIGHT_NORMAL,
                               titleWidth, false);
            c.width -= before - c.title.width;
        } else {
            c.width -= artGap + before;
            c.title = {};
        }
    }
    c.key += std::to_wstring(g.dpi) + kSeparator + std::to_wstring((int)height) +
             kSeparator + std::to_wstring((int)room);
    return c;
}

// ---- window ---------------------------------------------------------------------

using float2 = winrt::Windows::Foundation::Numerics::float2;
using float3 = winrt::Windows::Foundation::Numerics::float3;

// The island the point is on: 0 the middle one, 1 the left corner's, 2 the right
// corner's, -1 none.
int BodyAt(Island const& g, POINT pt) {
    if (g.centerOn &&
        InShape(g.shape, pt.x - g.islandRect.left, pt.y - g.islandRect.top, g.bottom)) {
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        Side const& d = g.sides[i];
        if (d.on && InShape(d.shape, pt.x - d.rect.left, pt.y - d.rect.top, g.bottom)) {
            return i + 1;
        }
    }
    return -1;
}

bool OnIsland(Island const& g, POINT pt) {
    return BodyAt(g, pt) >= 0;
}

// Keeps the taskbar's region box (the pill pieces with their margins) while
// the taskbar is out; until one is seen, a stand-in where the pill is aligned.
void NotePill(Island& g, RECT const& monitor) {
    RECT window, box;
    bool hasWindow = GetWindowRect(g.taskbar, &window);
    if (hasWindow && !autohide::Collapsed(g.taskbar) &&
        GetWindowRgnBox(g.taskbar, &box) > NULLREGION &&
        // Not the strip explorer leaves while the taskbar slides.
        box.bottom - box.top >= (window.bottom - window.top) / 2) {
        UINT dpi = GetDpiForWindow(g.taskbar);
        // The window is at the edge once it's out.
        LONG top = g.bottom ? monitor.bottom - (window.bottom - window.top) : monitor.top;
        g.pillBox = {window.left + box.left, top + box.top, window.left + box.right,
                     top + box.bottom};
        g.pillRadius = (float)settings::g.pill.cornerRadius * (dpi ? dpi : 96) / 96.0f;
        g.pillSeen = true;
        return;
    }
    if (g.pillSeen) {
        return;
    }
    int height = hasWindow ? window.bottom - window.top : (int)(g.shape.height * 2);
    int width = (monitor.right - monitor.left) * 3 / 5;
    int left = monitor.left + (monitor.right - monitor.left - width) / 2;
    if (auto align = settings::PillAlign(); align != settings::Align::kCenter) {
        left = align == settings::Align::kLeft ? monitor.left : monitor.right - width;
    }
    int top = g.bottom ? monitor.bottom - height : monitor.top;
    g.pillBox = {left, top, left + width, top + height};
    g.pillRadius = height / 2.0f;
}

// The drawn pill: the region box less its pieces' outer margins.
RECT PillRect(Island const& g) {
    RECT r = g.pillBox;
    if (g.pillSeen) {
        UINT dpi = GetDpiForWindow(g.taskbar);
        double s = (dpi ? dpi : 96) / 96.0;
        r.left += (LONG)std::lround(theme::kAppsMargin.Left * s);
        r.top += (LONG)std::lround(theme::kAppsMargin.Top * s);
        r.right -= (LONG)std::lround(settings::TrayMargin().Right * s);
        r.bottom -= (LONG)std::lround(theme::kAppsMargin.Bottom * s);
    }
    return r;
}

// The shape at one end of the morph, in window pixels.
struct Pose {
    float2 offset, size, radius;
    float3 fillets[2];  // offsets
};

Pose IslandPose(Island const& g) {
    Shape const& s = g.shape;
    float left = (float)(g.islandRect.left - g.window.left);
    float top = (float)(g.islandRect.top - g.window.top);
    float body = s.width - 2 * s.flare;
    Pose p;
    // The corners at the edge round past the window, so the body meets the
    // edge square.
    p.offset = {left + s.flare, g.bottom ? top : top - s.radius};
    p.size = {body, s.height + s.radius};
    p.radius = {s.radius, s.radius};
    float fillet = g.bottom ? top + s.height - g.filletSize : top;
    p.fillets[0] = {left, fillet, 0};
    p.fillets[1] = {left + s.flare + body, fillet, 0};
    return p;
}

// The fillets shrink into the pill's corners at the edge.
Pose PillPose(Island const& g) {
    RECT r = PillRect(g);
    float left = (float)(r.left - g.window.left);
    float top = (float)(r.top - g.window.top);
    float width = (float)(r.right - r.left);
    float height = (float)(r.bottom - r.top);
    float radius = std::max(0.0f, std::min({g.pillRadius, width / 2, height / 2}));
    Pose p;
    p.offset = {left, top};
    p.size = {width, height};
    p.radius = {radius, radius};
    float fillet = g.bottom ? top + height - g.filletSize : top;
    p.fillets[0] = {left - g.filletSize, fillet, 0};
    p.fillets[1] = {left + width, fillet, 0};
    return p;
}

// Both ends of the morph, for the expressions that put the shape between them
// by its progress.
void Place(Island& g) {
    auto put = [&](PCWSTR end, Pose const& p) {
        std::wstring name(end);
        g.morphState.InsertVector2(name + L"Offset", p.offset);
        g.morphState.InsertVector2(name + L"Size", p.size);
        g.morphState.InsertVector2(name + L"Radius", p.radius);
        g.morphState.InsertVector3(name + L"Fillet0", p.fillets[0]);
        g.morphState.InsertVector3(name + L"Fillet1", p.fillets[1]);
    };
    put(L"Island", IslandPose(g));
    put(L"Pill", PillPose(g));
}

// Everything the morph changes follows its one progress: the size, the color
// and the fades keep the same timing, and a turn midway runs back the same
// way. The shape turns from black into the pill's tint as it grows and, over
// the second half, fades onto the glass pill arriving under it; the content
// is gone early on the way out and comes back late on the way in.
// The hover transparency multiplies both fades: `Hover` runs 0 to 1 as the
// pointer comes to rest on the island, toward `HoverOpacity`.
void Bind(Island& g) {
    auto& m = g.morphState;
    m.InsertScalar(L"Progress", 0);
    m.InsertScalar(L"Hover", 0);
    m.InsertScalar(L"HoverOpacity", 1 - settings::g.island.hoverTransparency / 100.0f);
    m.InsertColor(L"Black", IslandFill());
    m.InsertColor(L"Pill", PillColor());
    Place(g);
    auto bind = [&](wuc::CompositionObject const& target, PCWSTR property,
                    std::wstring const& expression) {
        auto e = g.compositor.CreateExpressionAnimation(expression);
        e.SetReferenceParameter(L"m", m);
        target.StartAnimation(property, e);
    };
    bind(g.body, L"Offset", L"Lerp(m.IslandOffset, m.PillOffset, m.Progress)");
    bind(g.body, L"Size", L"Lerp(m.IslandSize, m.PillSize, m.Progress)");
    bind(g.body, L"CornerRadius", L"Lerp(m.IslandRadius, m.PillRadius, m.Progress)");
    bind(g.fill, L"Color", L"ColorLerpRGB(m.Black, m.Pill, m.Progress)");
    for (int i = 0; i < 2; i++) {
        std::wstring n = std::to_wstring(i);
        bind(g.fillets[i], L"Offset",
             L"Lerp(m.IslandFillet" + n + L", m.PillFillet" + n + L", m.Progress)");
        bind(g.fillets[i], L"Scale",
             L"Lerp(Vector3(1, 1, 1), Vector3(0, 0, 1), m.Progress)");
    }
    std::wstring hover = L" * Lerp(1, m.HoverOpacity, m.Hover)";
    bind(g.shapes, L"Opacity",
         L"Clamp((1 - m.Progress) / " + std::to_wstring(1 - theme::kIslandShapeFade) +
             L", 0, 1)" + hover);
    bind(g.content, L"Opacity",
         L"Clamp(1 - m.Progress / " + std::to_wstring(theme::kIslandContentOut) +
             L", 0, 1)" + hover);
    // The corner islands go as the middle one's content does, early on the way
    // out, and come back late on the way in.
    for (auto& d : g.sides) {
        bind(d.root, L"Opacity",
             L"Clamp(1 - m.Progress / " + std::to_wstring(theme::kIslandContentOut) +
                 L", 0, 1)" + hover);
    }
}

// ---- the taskbar under the morph ----------------------------------------------
// While the island grows, the taskbar is out already but unseen, so the only
// motion is the island's: the taskbar shows as the island fades onto it, and
// goes the same way as the island takes over from it. The opacity is on its
// content's composition visual; none of its XAML changes.

wuc::Visual BarVisual(Island& g) {
    auto root = core::TaskbarRoot(g.taskbar);
    auto content = root ? root.Content() : nullptr;
    wuc::Visual visual =
        content ? Hosting::ElementCompositionPreview::GetElementVisual(content) : nullptr;
    if (visual != g.bar) {
        g.bar = visual;
        g.reveal = nullptr;
    }
    return g.bar;
}

void StopReveal(Island& g) {
    if (g.bar) {
        g.bar.StopAnimation(L"Opacity");
    }
    if (g.reveal) {
        g.reveal.StopAnimation(L"Progress");
    }
}

// Unseen until a morph shows it.
void Veil(Island& g) {
    if (auto bar = BarVisual(g)) {
        StopReveal(g);
        bar.Opacity(0);
    }
}

// As Windows shows it; `out` is where the morph stands.
void Unveil(Island& g, bool out) {
    StopReveal(g);
    if (g.reveal) {
        g.reveal.InsertScalar(L"Progress", out ? 1.0f : 0.0f);
    }
    if (g.bar) {
        g.bar.Opacity(1);
    }
}

// Over the part of the morph where the island fades the taskbar rises, so the
// two cross. Started with the island's morph, with its curve and length.
void RevealWithMorph(Island& g, bool out, wf::TimeSpan length) {
    auto bar = BarVisual(g);
    if (!bar) {
        return;
    }
    auto compositor = bar.Compositor();
    if (!g.reveal) {
        g.reveal = compositor.CreatePropertySet();
        g.reveal.InsertScalar(L"Progress", out ? 0.0f : 1.0f);
    }
    auto opacity = compositor.CreateExpressionAnimation(
        L"Clamp((r.Progress - f) / (1 - f), 0, 1)");
    opacity.SetReferenceParameter(L"r", g.reveal);
    opacity.SetScalarParameter(L"f", theme::kIslandShapeFade);
    bar.StartAnimation(L"Opacity", opacity);
    auto const& e = theme::kIslandEase;
    auto progress = compositor.CreateScalarKeyFrameAnimation();
    progress.InsertExpressionKeyFrame(0, L"this.StartingValue");
    progress.InsertKeyFrame(
        1, out ? 1.0f : 0.0f,
        compositor.CreateCubicBezierEasingFunction({e[0], e[1]}, {e[2], e[3]}));
    progress.Duration(length);
    g.reveal.StartAnimation(L"Progress", progress);
}

// At one end of the morph, without one running.
void Settle(Island& g, bool out) {
    g.morphState.StopAnimation(L"Progress");
    g.morphState.InsertScalar(L"Progress", out ? 1.0f : 0.0f);
    Unveil(g, out);
}

// Idle, the island's shape (which is also what DWM shows); morphing, the
// whole window the shape moves over; out, only where the island sticks out
// of the pill, usually nothing, so the taskbar gets the pointer.
// A corner island's place while it counts, else nothing.
RECT SideRect(Island const& g, int i) {
    return g.sides[i].on ? g.sides[i].rect : RECT{};
}

void SetRegion(Island& g) {
    RegionKey key{g.phase, g.window, g.centerOn ? g.islandRect : RECT{}, g.pillBox,
                  {SideRect(g, 0), SideRect(g, 1)}};
    if (key == g.region) {
        return;
    }
    int width = g.window.right - g.window.left;
    int height = g.window.bottom - g.window.top;
    HRGN rgn = nullptr;
    if (g.phase == Phase::kExpanding || g.phase == Phase::kCollapsing) {
        rgn = CreateRectRgn(0, 0, width, height);
    } else if ((rgn = CreateRectRgn(0, 0, 0, 0))) {
        auto add = [&](Shape const& shape, RECT const& at) {
            if (HRGN part = Region(shape, g.bottom)) {
                OffsetRgn(part, at.left - g.window.left, at.top - g.window.top);
                CombineRgn(rgn, rgn, part, RGN_OR);
                DeleteObject(part);
            }
        };
        if (g.centerOn) {
            add(g.shape, g.islandRect);
        }
        // The corner ones are gone while the taskbar is out.
        for (auto const& d : g.sides) {
            if (d.on && g.phase == Phase::kIdle) {
                add(d.shape, d.rect);
            }
        }
        if (g.phase == Phase::kOut && !g.pillSeen) {
            SetRectRgn(rgn, 0, 0, 0, 0);
        } else if (g.phase == Phase::kOut) {
            RECT p = g.pillBox;
            OffsetRect(&p, -g.window.left, -g.window.top);
            if (HRGN pill = CreateRectRgnIndirect(&p)) {
                CombineRgn(rgn, rgn, pill, RGN_DIFF);
                DeleteObject(pill);
            }
        }
    }
    if (rgn && SetWindowRgn(g.hwnd, rgn, TRUE)) {
        g.region = key;
    } else if (rgn) {
        DeleteObject(rgn);
    }
}

void Resize(Island& g, Shape const& shape) {
    g.drawn.clear();
    SIZE size{(LONG)shape.width, (LONG)shape.height};
    check_hresult(g.surface.as<ICompositionDrawingSurfaceInterop>()->Resize(size));
    g.content.Size({shape.width, shape.height});
    float f = std::max(1.0f, shape.flare);
    if (f != g.filletSize) {
        SIZE square{(LONG)f, (LONG)f};
        for (auto& surface : g.filletSurfaces) {
            check_hresult(surface.as<ICompositionDrawingSurfaceInterop>()->Resize(square));
        }
        g.filletSize = f;
        DrawFillets(g);
        for (auto& fillet : g.fillets) {
            fillet.Size({f, f});
        }
        // Each shrinks toward the corner it shares with the body at the edge.
        float edge = g.bottom ? f : 0;
        g.fillets[0].CenterPoint({f, edge, 0});
        g.fillets[1].CenterPoint({0, edge, 0});
    }
    g.shape = shape;
}

// A corner island (`left` or right) to `shape`; its one fillet is on its inner
// side, the body runs past the screen's side and edge so those corners are square.
void ResizeSide(Island& g, Side& d, Shape const& shape, bool left) {
    d.drawn.clear();
    SIZE size{(LONG)shape.width, (LONG)shape.height};
    check_hresult(d.surface.as<ICompositionDrawingSurfaceInterop>()->Resize(size));
    d.content.Size({shape.width, shape.height});
    float f = std::max(1.0f, shape.flare);
    if (f != d.filletSize) {
        SIZE square{(LONG)f, (LONG)f};
        check_hresult(d.filletSurface.as<ICompositionDrawingSurfaceInterop>()->Resize(square));
        d.filletSize = f;
        auto fillet = Fillet(g.d2d.get(), f, left, g.bottom);
        Paint(d.filletSurface, [&](ID2D1DeviceContext* dc) {
            dc->FillGeometry(fillet.get(), Brush(dc, IslandFill()).get(), nullptr);
        });
        d.fillet.Size({f, f});
    }
    float r = shape.radius, body = shape.width - shape.flare;
    d.body.Offset({left ? -r : shape.flare, g.bottom ? 0 : -r});
    d.body.Size({body + r, shape.height + r});
    d.body.CornerRadius({r, r});
    d.bodyVisual.Size({shape.width, shape.height});
    d.fillet.Offset({left ? body : 0, g.bottom ? shape.height - f : 0, 0});
    d.shape = shape;
}

// A corner island with nothing to show keeps no pixels.
void EmptySide(Side& d) {
    if (d.shape.width > 0) {
        check_hresult(d.surface.as<ICompositionDrawingSurfaceInterop>()->Resize({1, 1}));
    }
    d.shape = {};
    d.drawn.clear();
    d.rect = {};
}

// Sizes, places and draws the islands for what they show now: the middle one and
// those in the left and right corners; false if that's nothing. While a morph runs
// only the middle one's content may change; the rest waits for its end.
bool Layout(Island& g) {
    RECT monitor;
    if (!MonitorRect(g, monitor)) {
        return false;
    }
    WatchPlaces();
    NotePlaceMonitor(g, core::MonitorOf(g.taskbar));
    float s = g.dpi / 96.0f;
    float height = std::round(settings::g.island.height * s);
    float pad = std::round(height * (float)theme::kIslandPadding);
    float fixed = std::round(settings::g.island.width * s);
    Content c = Measure(g, height, fixed > 0 ? fixed - 2 * pad : 0, settings::Slot::kCenter);
    Content corner[2] = {Measure(g, height, 0, settings::Slot::kLeft),
                         Measure(g, height, 0, settings::Slot::kRight)};
    bool centerOn = c.width > 0;
    if (!centerOn && corner[0].width <= 0 && corner[1].width <= 0) {
        return false;
    }
    Shape shape;
    shape.height = height;
    shape.radius = height / 2;
    shape.flare =
        std::min(std::round(theme::kIslandFlare * s), height - shape.radius);
    float body = fixed > 0 ? std::max(fixed, height)
                           : std::max({c.width + 2 * pad, height,
                                       std::round((float)theme::kIslandMinWidth * s)});
    shape.width = std::min(std::ceil(body) + 2 * shape.flare,
                           (float)(monitor.right - monitor.left));
    if (g.phase == Phase::kExpanding || g.phase == Phase::kCollapsing) {
        if (centerOn && g.centerOn && shape == g.shape && c.key != g.drawn) {
            DrawContent(g, c, g.surface, g.shape);
            g.drawn = c.key;
        }
        return true;
    }
    if (!(shape == g.shape)) {
        Resize(g, shape);
    }
    // Without anything of its own the middle island is still the morph's start.
    g.centerOn = centerOn;
    g.bodyVisual.IsVisible(centerOn);
    g.fillets[0].IsVisible(centerOn);
    g.fillets[1].IsVisible(centerOn);
    g.content.IsVisible(centerOn);

    int top = g.bottom ? monitor.bottom - (int)shape.height : monitor.top;
    for (int i = 0; i < 2; i++) {
        Side& d = g.sides[i];
        bool on = corner[i].width > 0;
        if (on) {
            Shape ss;
            ss.height = height;
            ss.radius = height / 2;
            ss.flare = shape.flare;
            ss.flushLeft = i == 0;
            ss.flushRight = i == 1;
            ss.width = std::min(std::ceil(std::max(corner[i].width + 2 * pad, height)) + ss.flare,
                                (float)(monitor.right - monitor.left) / 3);
            if (!(ss == d.shape)) {
                ResizeSide(g, d, ss, i == 0);
            }
            int w = (int)ss.width;
            int x = i == 0 ? monitor.left : monitor.right - w;
            d.rect = {x, top, x + w, top + (int)height};
        } else if (d.on || d.shape.width > 0) {
            EmptySide(d);
        }
        d.on = on;
        d.root.IsVisible(on);
    }

    // The middle one where it's placed, but clear of the corner ones.
    int width = (int)shape.width;
    int x = PlacedLeft(g, monitor, width);
    int apart = (int)std::lround(theme::kIslandApart * s);
    int lo = g.sides[0].on ? g.sides[0].rect.right + apart : (int)monitor.left;
    int hi = g.sides[1].on ? g.sides[1].rect.left - apart : (int)monitor.right;
    if (hi - lo >= width) {
        x = std::clamp(x, lo, hi - width);
    }
    g.islandRect = {x, top, x + width, top + (int)shape.height};
    // Past the edge: the row above the top one, the row below the bottom one.
    int beyond = g.bottom ? monitor.bottom : monitor.top - 1;
    g.sharedEdge = false;
    auto across = [&](RECT const& r) {
        for (int px : {r.left, (r.left + r.right) / 2, r.right - 1}) {
            g.sharedEdge |= MonitorFromPoint({px, beyond}, MONITOR_DEFAULTTONULL) != nullptr;
        }
    };
    if (centerOn) {
        across(g.islandRect);
    }
    for (auto const& d : g.sides) {
        if (d.on) {
            across(d.rect);
        }
    }
    NotePill(g, monitor);
    RECT window;
    UnionRect(&window, &g.islandRect, &g.pillBox);
    for (auto const& d : g.sides) {
        if (d.on) {
            UnionRect(&window, &window, &d.rect);
        }
    }
    if (!EqualRect(&window, &g.window)) {
        g.window = window;
        int w = window.right - window.left, h = window.bottom - window.top;
        SetWindowPos(g.hwnd, nullptr, window.left, window.top, w, h,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        float2 extent{(float)w, (float)h};
        g.root.Size(extent);
        g.bodyVisual.Size(extent);
    }
    g.content.Offset({(float)(g.islandRect.left - g.window.left),
                      (float)(g.islandRect.top - g.window.top), 0});
    for (auto& d : g.sides) {
        if (d.on) {
            d.root.Offset({(float)(d.rect.left - g.window.left),
                           (float)(d.rect.top - g.window.top), 0});
        }
    }
    Place(g);
    SetRegion(g);
    if (centerOn && c.key != g.drawn) {
        DrawContent(g, c, g.surface, g.shape);
        g.drawn = c.key;
    }
    for (int i = 0; i < 2; i++) {
        Side& d = g.sides[i];
        if (d.on && corner[i].key != d.drawn) {
            DrawContent(g, corner[i], d.surface, d.shape);
            d.drawn = corner[i].key;
        }
    }
    return true;
}

bool LayoutRecovering(Island& g) {
    try {
        return Layout(g);
    } catch (winrt::hresult_error const& e) {
        if (!DeviceLost(e.code())) {
            throw;
        }
        Wh_Log(L"Island device lost");
    }
    Recover(g);
    g.shape = {};
    g.filletSize = 0;
    for (auto& d : g.sides) {
        d.shape = {};
        d.filletSize = 0;
    }
    return Layout(g);
}

void Track(HWND hwnd) {
    TRACKMOUSEEVENT track{.cbSize = sizeof(track), .dwFlags = TME_LEAVE, .hwndTrack = hwnd};
    TrackMouseEvent(&track);
}

void FadeIn(Island& g) {
    auto fade = g.compositor.CreateScalarKeyFrameAnimation();
    fade.InsertKeyFrame(0, 0);
    fade.InsertKeyFrame(1, 1);
    fade.Duration(std::chrono::milliseconds(theme::kIslandFadeMs));
    int delay = settings::g.autohide.animation > 0 ? settings::g.autohide.animation
                                                   : theme::kIslandFadeDelayMs;
    fade.DelayTime(std::chrono::milliseconds(delay));
    fade.DelayBehavior(wuc::AnimationDelayBehavior::SetInitialValueBeforeDelay);
    g.root.StartAnimation(L"Opacity", fade);
}

// Faded while the pointer rests on the island and the taskbar is in, solid
// again as the pointer leaves or the island starts to grow. Solid too while
// Ctrl is held or the island is dragged, so it can be seen where it goes.
void SetHover(Island& g) {
    bool hover = settings::g.island.hoverTransparency > 0 && g.inside && !g.ctrl &&
                 !g.dragging && g.phase == Phase::kIdle && autohide::Collapsed(g.taskbar);
    if (hover == g.hover) {
        return;
    }
    g.hover = hover;
    core::Guard(L"Island hover", [&] {
        auto const& e = theme::kIslandHoverEase;
        auto fade = g.compositor.CreateScalarKeyFrameAnimation();
        // From wherever a fade the other way left it.
        fade.InsertExpressionKeyFrame(0, L"this.StartingValue");
        fade.InsertKeyFrame(
            1, hover ? 1.0f : 0.0f,
            g.compositor.CreateCubicBezierEasingFunction({e[0], e[1]}, {e[2], e[3]}));
        fade.Duration(std::chrono::milliseconds(hover ? theme::kIslandHoverInMs
                                                      : theme::kIslandHoverOutMs));
        g.morphState.StartAnimation(L"Hover", fade);
    });
}

// Solid at once, for an island shown again.
void ClearHover(Island& g) {
    g.hover = false;
    g.morphState.StopAnimation(L"Hover");
    g.morphState.InsertScalar(L"Hover", 0);
}

// Idle, right under the taskbar, so the taskbar covers it when it's out. In
// click mode over it instead: the hidden taskbar's edge strip would take the
// island's top pixels, and a click there would land on the hidden taskbar.
// Over it too while Windows keeps the taskbar out of the topmost band (a
// borderless window maximized to the whole monitor looks fullscreen to it):
// under the taskbar would then be under that window.
void StackIdle(Island& g, UINT flags = 0) {
    bool taskbarOnTop = GetWindowLongPtrW(g.taskbar, GWL_EXSTYLE) & WS_EX_TOPMOST;
    HWND after = settings::g.island.clickToReveal || !taskbarOnTop ? HWND_TOPMOST : g.taskbar;
    SetWindowPos(g.hwnd, after, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | flags);
}

// Over the taskbar, for a morph across it.
void Raise(Island& g) {
    SetWindowPos(g.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ---- morph ----------------------------------------------------------------------

void SinkLiftedTaskbar(Island& g);
bool CoversMonitor(Island const& g, HWND window);

void OnMorphed(int id, int morph) {
    Island* p = FindIf([&](Island const& g) { return g.id == id; });
    if (!p || p->morph != morph) {
        return;
    }
    auto& g = *p;
    core::DiagLine(L"Morphed %d phase %d", g.id, (int)g.phase);
    g.batch = nullptr;
    core::Guard(L"Taskbar reveal", [&] {
        if (g.phase == Phase::kCollapsing) {
            // In now. Its window comes out at once, a moment before the next
            // morph starts; the taskbar shows only with that morph.
            Veil(g);
        } else {
            Unveil(g, g.phase == Phase::kExpanding);
        }
    });
    if (g.phase == Phase::kExpanding) {
        g.phase = Phase::kOut;
        Sync(g);
    } else if (g.phase == Phase::kCollapsing) {
        g.phase = Phase::kIdle;
        SinkLiftedTaskbar(g);
        Sync(g);
        SetHover(g);
    }
}

// The taskbar StartExpand lifted over a fullscreen window, in again (or the
// island hid meanwhile): out of the topmost band and right under the window in
// front, where the shell had it, so the edge strip it leaves on the screen
// doesn't line that window. Only while that window still fills the monitor:
// otherwise the taskbar is where it belongs anyway, topmost, and taking it out
// of that band would leave it under every maximized window.
void SinkLiftedTaskbar(Island& g) {
    if (!std::exchange(g.liftedTaskbar, false)) {
        return;
    }
    HWND front = GetForegroundWindow();
    if (!front || front == g.taskbar || front == g.hwnd || !CoversMonitor(g, front) ||
        (GetWindowLongPtrW(front, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return;
    }
    constexpr UINT kFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    SetWindowPos(g.taskbar, HWND_NOTOPMOST, 0, 0, 0, 0, kFlags);
    SetWindowPos(g.taskbar, front, 0, 0, 0, 0, kFlags);
}

// The shape toward the pill (`out`) or the island, from wherever it is now, so
// a turn midway carries on smoothly. Its length is the taskbar's slide, so the
// shape fades onto the pill just as the taskbar arrives. All on the compositor.
void Morph(Island& g, bool out) {
    auto const& e = theme::kIslandEase;
    auto ease = g.compositor.CreateCubicBezierEasingFunction({e[0], e[1]}, {e[2], e[3]});
    if (g.batch) {
        g.batch.Completed(g.batchDone);
    }
    auto batch = g.compositor.CreateScopedBatch(wuc::CompositionBatchTypes::Animation);
    int morph = ++g.morph;
    // The pill's tint as it is now (the theme or the setting may have changed).
    g.morphState.InsertColor(L"Pill", PillColor());
    wf::TimeSpan length = std::chrono::milliseconds(MorphMs());
    auto progress = g.compositor.CreateScalarKeyFrameAnimation();
    progress.InsertExpressionKeyFrame(0, L"this.StartingValue");
    progress.InsertKeyFrame(1, out ? 1.0f : 0.0f, ease);
    progress.Duration(length);
    g.morphState.StartAnimation(L"Progress", progress);
    g.batchDone = batch.Completed([id = g.id, morph](auto&&, auto&&) {
        core::Guard(L"Island morph", [&] { OnMorphed(id, morph); });
    });
    batch.End();
    g.batch = batch;
    core::Guard(L"Taskbar reveal", [&] { RevealWithMorph(g, out, length); });
}

// The taskbar is coming out: from the island, or from where a shrink left
// off, to the pill, over the taskbar.
void StartExpand(Island& g) {
    core::DiagLine(L"StartExpand %d from %d", g.id, (int)g.phase);
    if (g.phase == Phase::kIdle) {
        // The pill as it is now.
        core::Guard(L"Island layout", [&] { LayoutRecovering(g); });
    }
    g.phase = Phase::kExpanding;
    SetHover(g);
    Raise(g);
    // Out of the topmost band (a window fills the monitor, see StackIdle) the
    // taskbar would come out under that window: it goes over it, as the
    // island shows it coming. Into the topmost band, not just to the top: a
    // browser in F11 marks its window fullscreen with the shell, which then
    // keeps the taskbar under it, so the taskbar came out unseen behind it.
    // Back under the window in front as it goes in (SinkLiftedTaskbar).
    if (!(GetWindowLongPtrW(g.taskbar, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        g.liftedTaskbar = true;
        SetWindowPos(g.taskbar, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    SetRegion(g);
    Morph(g, true);
}

// The taskbar is going in: from the pill, or from where a growth left off,
// back to the island, over the leaving taskbar.
void StartCollapse(Island& g) {
    POINT cpt{};
    GetCursorPos(&cpt);
    core::DiagLine(L"StartCollapse %d from %d at %d,%d", g.id, (int)g.phase, cpt.x, cpt.y);
    if (g.phase == Phase::kOut) {
        // On the pill as last seen.
        core::Guard(L"Island layout", [&] { LayoutRecovering(g); });
    }
    g.phase = Phase::kCollapsing;
    // A pointer already on the island has to leave and come back, so a
    // taskbar that went in with the pointer there doesn't bounce out again.
    POINT pt;
    g.inside = GetCursorPos(&pt) && OnIsland(g, pt);
    g.reaching = false;
    if (g.inside) {
        Track(g.hwnd);
    }
    SetHover(g);
    Raise(g);
    SetRegion(g);
    Morph(g, false);
}

void Show(Island& g) {
    g.shown = true;
    // As the taskbar is now: the island, or nothing over a taskbar that's out.
    bool out = !autohide::Collapsed(g.taskbar);
    g.phase = out ? Phase::kOut : Phase::kIdle;
    core::DiagLine(L"Show %d taskbar %p out %d hidden %d shared %d", g.id, g.taskbar, out,
                   autohide::LooksHidden(g.taskbar), g.sharedEdge);
    Settle(g, out);
    if (!out) {
        core::Guard(L"Taskbar reveal", [&] { Veil(g); });
    }
    SetRegion(g);
    core::Guard(L"Island fade", [&] { FadeIn(g); });
    StackIdle(g, SWP_SHOWWINDOW);
    POINT pt;
    g.inside = GetCursorPos(&pt) && OnIsland(g, pt);
    g.reaching = false;
    if (g.inside) {
        Track(g.hwnd);
    }
    SetHover(g);
}

void EndDrag(Island& g);

void Hide(Island& g) {
    EndDrag(g);
    KillTimer(g.hwnd, kCrossingTimer);
    g.shown = g.inside = g.reaching = false;
    g.phase = Phase::kIdle;
    g.morph++;  // a running morph's end no longer counts, so its sink comes here
    SinkLiftedTaskbar(g);
    ShowWindow(g.hwnd, SW_HIDE);
    // Shown again from transparent, not with the last frame.
    core::Guard(L"Island fade", [&] {
        g.root.StopAnimation(L"Opacity");
        g.root.Opacity(0);
        Settle(g, false);
        ClearHover(g);
    });
}

// ---- inputs ---------------------------------------------------------------------

void OnClock(winrt::hstring const& text) {
    std::wstring value(text);
    if (!g_islands.empty() && g_clock != value) {
        g_clock = std::move(value);
        SyncAll();
    }
}

// Explorer writes the time into this text block; the islands copy it.
void FindClock() {
    if (!settings::g.island.showClock || g_clockText.get()) {
        return;
    }
    auto root = core::TaskbarRoot(core::PrimaryTaskbar());
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    auto text = core::FindDescendant(
                    content, [](auto& e) { return e.Name() == L"TimeInnerTextBlock"; },
                    40)
                    .try_as<Controls::TextBlock>();
    if (!text) {
        return;
    }
    g_clockText = text;
    g_clock = text.Text();
    // The tray's clock format watches this text too.
    core::Watch(
        text, Controls::TextBlock::TextProperty(),
        [](DependencyObject const& sender) {
            OnClock(sender.as<Controls::TextBlock>().Text());
        },
        L"island");
}

void DecodeArt(Island& g, wss::IBuffer const& bytes) {
    com_ptr<IWICStream> stream;
    check_hresult(g.wic->CreateStream(stream.put()));
    check_hresult(stream->InitializeFromMemory(bytes.data(), bytes.Length()));
    com_ptr<IWICBitmapDecoder> decoder;
    check_hresult(g.wic->CreateDecoderFromStream(
        stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.put()));
    com_ptr<IWICBitmapFrameDecode> frame;
    check_hresult(decoder->GetFrame(0, frame.put()));
    UINT width = 0, height = 0;
    check_hresult(frame->GetSize(&width, &height));
    if (!width || !height) {
        return;
    }
    auto source = frame.as<IWICBitmapSource>();
    UINT shorter = std::min(width, height);
    if (shorter > kArtSize) {
        width = std::max<UINT>(1, (UINT)((uint64_t)width * kArtSize / shorter));
        height = std::max<UINT>(1, (UINT)((uint64_t)height * kArtSize / shorter));
        com_ptr<IWICBitmapScaler> scaler;
        check_hresult(g.wic->CreateBitmapScaler(scaler.put()));
        check_hresult(scaler->Initialize(source.get(), width, height,
                                         WICBitmapInterpolationModeFant));
        source = scaler.as<IWICBitmapSource>();
    }
    com_ptr<IWICFormatConverter> converter;
    check_hresult(g.wic->CreateFormatConverter(converter.put()));
    check_hresult(converter->Initialize(source.get(), GUID_WICPixelFormat32bppPBGRA,
                                        WICBitmapDitherTypeNone, nullptr, 0,
                                        WICBitmapPaletteTypeMedianCut));
    // Decoded now, while the bytes are there.
    com_ptr<IWICBitmap> bitmap;
    check_hresult(g.wic->CreateBitmapFromSource(converter.get(), WICBitmapCacheOnLoad,
                                                bitmap.put()));
    g.art = bitmap;
    g.artWidth = width;
    g.artHeight = height;
}

void ShowMedia(Island& g, media::Snapshot const& s) {
    std::wstring title = s.visible ? std::wstring(s.title) : std::wstring();
    bool changed = s.visible != g.mediaVisible || title != g.title;
    g.mediaVisible = s.visible;
    g.title = std::move(title);
    if (s.artChanged || !s.visible) {
        bool had = (bool)g.art;
        g.art = nullptr;
        if (s.visible && s.artBytes) {
            core::Guard(L"Island art", [&] { DecodeArt(g, s.artBytes); });
        }
        if (had || g.art) {
            g.artId++;
            changed = true;
        }
    }
    if (changed) {
        Sync(g);
    }
}

void OnMedia(media::Snapshot const& s) {
    for (auto& g : g_islands) {
        if (g.ready) {
            core::Guard(L"Island media", [&] { ShowMedia(g, s); });
        }
    }
}

void RetryBuild(HWND taskbar);

// The taskbar's real state picks the morph; a quick in and out that is over
// before this runs leaves the island as it was.
void OnCollapsedChanged(HWND hwnd) {
    Island* p = Of(hwnd);
    if (!p) {
        RetryBuild(hwnd);
        return;
    }
    auto& g = *p;
    core::Guard(L"Island state", [&] {
        bool collapsed = autohide::Collapsed(hwnd);
        bool in = g.phase == Phase::kIdle || g.phase == Phase::kCollapsing;
        if (g.shown && !collapsed && in) {
            StartExpand(g);
        } else if (g.shown && collapsed && !in) {
            StartCollapse(g);
        } else {
            Sync(g);
        }
    });
}

// The taskbar is about to show itself expanded or collapsed, before it draws
// that. Going out, it is hidden here, and the window's move starts the morph
// (an expand without the move is a focus change elsewhere that never comes
// out); coming back before it left, or going in, the morph starts now, in
// step with the taskbar's own change.
void OnExpandedChanged(HWND hwnd, bool expanded) {
    Island* p = Of(hwnd);
    if (!p || !p->shown) {
        return;
    }
    auto& g = *p;
    if (expanded && g.phase == Phase::kCollapsing) {
        StartExpand(g);
    } else if (expanded && g.phase == Phase::kIdle) {
        // Already out by its window: nothing else will start the morph.
        if (!autohide::Collapsed(hwnd)) {
            StartExpand(g);
        } else {
            Veil(g);
        }
    } else if (!expanded && (g.phase == Phase::kOut || g.phase == Phase::kExpanding)) {
        StartCollapse(g);
    } else if (!expanded && g.phase == Phase::kIdle) {
        Unveil(g, false);
    }
}

// The island shows the taskbar's slide; its own takes no time.
bool StandsIn(HWND hwnd) {
    Island* g = OnIslandThread() ? Of(hwnd) : nullptr;
    return g && g->shown;
}

void RecheckFullscreen(Island& g);

// The taskbar StartExpand lifted over a fullscreen window stays over it until
// it is in again (SinkLiftedTaskbar). Otherwise a click on it, with the window
// coming back to the front after, has the shell put it back under a browser in
// F11 for a moment.
bool KeepsTaskbarTopmost(HWND hwnd) {
    Island* g = OnIslandThread() ? Of(hwnd) : nullptr;
    return g && g->liftedTaskbar;
}

// Explorer raises the taskbar as it comes out; a morph stays on top of it, and
// so does an idle island in click mode. A fullscreen app coming or going
// restacks it too (RecheckFullscreen).
void OnTaskbarRestacked(HWND hwnd) {
    Island* p = Of(hwnd);
    if (!p) {
        return;
    }
    RecheckFullscreen(*p);
    p = Of(hwnd);  // Sync may have hidden or rebuilt it
    if (!p) {
        return;
    }
    auto& g = *p;
    if (g.phase == Phase::kExpanding || g.phase == Phase::kCollapsing ||
        (g.shown && g.phase == Phase::kIdle && settings::g.island.clickToReveal)) {
        Raise(g);
    }
}

// Ctrl held: the island is being moved, or about to be.
bool CtrlDown() {
    return GetAsyncKeyState(VK_CONTROL) & 0x8000;
}

// The point is in the island's half at the screen edge: a pointer coming onto
// it from the monitor past that edge shows up there first.
bool EdgeHalf(Island const& g, POINT pt) {
    int fromEdge = g.bottom ? g.islandRect.bottom - 1 - pt.y : pt.y - g.islandRect.top;
    return fromEdge < (int)g.shape.height / 2;
}

// While the island shows, only the pointer at the edge over it brings the
// taskbar out. Past the edge another monitor is a way through, so a pointer
// crossing it brings nothing; only touching the island does. In click mode,
// and while Ctrl is held or the island is dragged, the pointer brings nothing.
bool RevealsAt(HWND hwnd, int x, bool& reveals) {
    Island* p = OnIslandThread() ? Of(hwnd) : nullptr;
    if (!p || !p->shown) {
        return false;
    }
    auto& g = *p;
    reveals = false;
    if (g.sharedEdge || settings::g.island.clickToReveal || g.dragging || CtrlDown()) {
        return true;
    }
    auto over = [&](RECT const& r) { return x >= r.left && x < r.right; };
    reveals = (g.centerOn && over(g.islandRect)) ||
              (g.sides[0].on && over(g.sides[0].rect)) || (g.sides[1].on && over(g.sides[1].rect));
    return true;
}

// In click mode, or with Ctrl held, the edge strip of a hidden taskbar brings
// nothing while its island shows, also as it shrinks back: only a click on
// the island does. The caller checks the taskbar is hidden.
bool HoldsHover(HWND hwnd) {
    Island* p = OnIslandThread() ? Of(hwnd) : nullptr;
    return p && p->shown && (settings::g.island.clickToReveal || p->dragging || CtrlDown());
}

// As if the pointer touched the screen edge. The tray keeps the taskbar out
// while the pointer is on it or the island under it, and takes it in after.
void Expand(Island& g) {
    // The window tells whether it's out.
    if (!autohide::Collapsed(g.taskbar)) {
        return;
    }
    HWND taskbar = g.taskbar;
    int id = g.id;
    bool asked = false;
    // The tray's slide runs inside; `g` may be gone after it.
    core::Guard(L"Island reveal", [&] { asked = autohide::Reveal(taskbar); });
    core::DiagLine(L"Reveal %d %p asked %d", id, taskbar, asked);
    Island* after = Of(taskbar);
    if (!asked && after && !std::exchange(after->unreachable, true)) {
        Wh_Log(L"Island: the taskbar can't be asked to come out");
    }
}

// The pointer came onto the island, or clicked it, to bring the taskbar out.
// The tray's slide runs inside: returns the island as it is after, null if
// it's gone.
Island* Reach(Island& g) {
    core::DiagLine(L"Reach %d", g.id);
    g.reaching = true;
    HWND taskbar = g.taskbar;
    Expand(g);
    return Of(taskbar);
}

// True while `window` still fills the island's monitor.
bool CoversMonitor(Island const& g, HWND window) {
    RECT monitor, rc;
    return window && IsWindow(window) && IsWindowVisible(window) && !IsIconic(window) &&
           MonitorRect(g, monitor) && GetWindowRect(window, &rc) && rc.left <= monitor.left &&
           rc.top <= monitor.top && rc.right >= monitor.right && rc.bottom >= monitor.bottom;
}

// A fullscreen app (a game, a video) on the monitor keeps the island away, as
// it does the taskbar. A maximized window is no such app, even one without a
// frame that Windows reports as one for filling the monitor. A window activated
// on another monitor leaves that as it is, unless the fullscreen app has gone
// meanwhile.
// A browser's window (Chromium's, which Edge and Electron apps share, or
// Firefox's): in F11 or a fullscreen video it fills the monitor as a game does,
// but the island stays and brings the taskbar out over it (StartExpand).
bool IsBrowserWindow(HWND window) {
    WCHAR name[64];
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           (wcscmp(name, L"Chrome_WidgetWin_1") == 0 || wcscmp(name, L"MozillaWindowClass") == 0);
}

void SetFullscreen(Island& g, HWND window) {
    g.fullscreenWindow = window;
    bool fullscreen = window != nullptr;
    if (fullscreen != g.fullscreen) {
        g.fullscreen = fullscreen;
        Sync(g);
    }
}

void OnActivated(Island& g, HWND window, bool rude) {
    bool here = window && MonitorFromWindow(window, MONITOR_DEFAULTTONULL) ==
                              core::MonitorOf(g.taskbar);
    if (here || !window) {
        SetFullscreen(g, rude && here && !IsZoomed(window) && !IsBrowserWindow(window) ? window : nullptr);
    } else if (!CoversMonitor(g, g.fullscreenWindow)) {
        SetFullscreen(g, nullptr);
    }
}

// The activation's word comes only as a window is activated, and a game mostly
// goes fullscreen after that, or switches to it from its settings. Windows takes
// the taskbar out of the topmost band then, which restacks it (OnTaskbarRestacked):
// the window in front is looked at again. Filling this monitor without being
// maximized, it is a fullscreen app; maximized, it is one only when Direct3D
// holds the screen (exclusive fullscreen). The idle island, topmost while the
// taskbar isn't, would otherwise stay over the game, and an exclusive one
// minimizes when a window comes over it.
void RecheckFullscreen(Island& g) {
    if (GetWindowLongPtrW(g.taskbar, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        if (!CoversMonitor(g, g.fullscreenWindow)) {
            SetFullscreen(g, nullptr);  // the game left fullscreen
        }
        return;
    }
    HWND front = GetForegroundWindow();
    if (!front || front == g.hwnd || front == g.taskbar ||
        MonitorFromWindow(front, MONITOR_DEFAULTTONULL) != core::MonitorOf(g.taskbar) ||
        !CoversMonitor(g, front) || IsBrowserWindow(front)) {
        return;
    }
    QUERY_USER_NOTIFICATION_STATE state{};
    bool d3d = SUCCEEDED(SHQueryUserNotificationState(&state)) &&
               state == QUNS_RUNNING_D3D_FULL_SCREEN;
    if (!IsZoomed(front) || d3d) {
        SetFullscreen(g, front);
    }
}

// ---- moving along the edge -----------------------------------------------------
// Ctrl and the left button on the idle island drag it along its edge; the
// place is saved as it's let go. Ctrl and a right click put it back in the
// middle.

void StartDrag(Island& g, POINT pt) {
    g.dragging = true;
    g.grab = pt.x - g.islandRect.left;
    SetCapture(g.hwnd);
    SetHover(g);
}

void Drag(Island& g, POINT pt) {
    RECT monitor;
    if (MonitorRect(g, monitor)) {
        SetPlace(g, monitor, pt.x - g.grab);
        core::Guard(L"Island layout", [&] { LayoutRecovering(g); });
    }
}

void EndDrag(Island& g) {
    if (!std::exchange(g.dragging, false)) {
        return;
    }
    if (GetCapture() == g.hwnd) {
        ReleaseCapture();
    }
    if (!g.placeId.empty()) {
        core::Guard(L"Island place", [&] { SavePlace(g); });
    }
    SetHover(g);
}

void ToMiddle(Island& g) {
    if (g.placeId.empty()) {
        return;
    }
    g_places[g.placeId] = kPlaceMiddle;
    core::Guard(L"Island place", [&] { SavePlace(g); });
    core::Guard(L"Island layout", [&] { LayoutRecovering(g); });
}

// Shown at its monitor's top or bottom taskbar while there is something to
// show; invisible while the taskbar is out.
void Sync(Island& g) {
    if (!g.ready) {
        return;
    }
    RECT monitor;
    bool want = !core::g_vertical && !g.fullscreen && MonitorRect(g, monitor);
    if (want && AtBottom(g.taskbar, monitor) != g.bottom) {
        // The taskbar went to the other edge: the island starts there afresh.
        if (g.shown) {
            Hide(g);
        }
        g.bottom = !g.bottom;
        g.shape = {};
        g.filletSize = 0;
        g.window = {};
        g.region = {};
        g.pillSeen = false;
        for (auto& d : g.sides) {
            EmptySide(d);
            d.filletSize = 0;
        }
    }
    if (want) {
        FindClock();
        bool laidOut = false;
        core::Guard(L"Island layout", [&] { laidOut = LayoutRecovering(g); });
        want = laidOut;
    }
    if (want && !g.shown) {
        Show(g);
    } else if (!want && g.shown) {
        Hide(g);
    } else if (want && g.phase == Phase::kIdle && !settings::g.island.clickToReveal) {
        // In click mode it went over the taskbar as it showed and stays
        // there (OnTaskbarRestacked); raising it on every change of what it
        // shows would put it over every other topmost window too.
        StackIdle(g);
    }
}

std::optional<LRESULT> Handle(Island& g, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST: {
            POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
            if ((g.phase == Phase::kExpanding || g.phase == Phase::kCollapsing) &&
                PtInRect(&g.pillBox, pt)) {
                // Passed on to the taskbar beneath, on this same thread.
                return HTTRANSPARENT;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            // While a morph runs the window is wider than the island.
            POINT pt;
            bool got = GetCursorPos(&pt);
            if (g.dragging) {
                if (got) {
                    Drag(g, pt);
                }
                return 0;
            }
            g.ctrl = wParam & MK_CONTROL;
            bool on = got && OnIsland(g, pt);
            Island* after = &g;
            if (on && !g.inside) {
                g.inside = true;
                Track(g.hwnd);
                // With Ctrl held it comes to move the island, not the taskbar.
                if (!settings::g.island.clickToReveal && !g.ctrl) {
                    if (g.sharedEdge && EdgeHalf(g, pt)) {
                        SetTimer(g.hwnd, kCrossingTimer, kCrossingMs, nullptr);
                    } else {
                        after = Reach(g);
                    }
                }
            } else if (!on) {
                g.inside = g.reaching = false;
                KillTimer(g.hwnd, kCrossingTimer);
            }
            if (after) {
                SetHover(*after);
            }
            return 0;
        }
        // A click, pressed and released on the idle island, brings the taskbar
        // out. In both modes: also an island the pointer stayed on as the
        // taskbar went in. Held while pressed, so the release comes here and
        // not to the taskbar coming out under it.
        case WM_LBUTTONDOWN: {
            POINT pt;
            if (g.shown && g.phase == Phase::kIdle && GetCursorPos(&pt) && OnIsland(g, pt)) {
                if (wParam & MK_CONTROL) {
                    if (BodyAt(g, pt) == 0) {
                        StartDrag(g, pt);
                    }
                    return 0;
                }
                g.pressed = true;
                SetCapture(g.hwnd);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g.dragging) {
                EndDrag(g);
                return 0;
            }
            if (!std::exchange(g.pressed, false)) {
                return 0;
            }
            ReleaseCapture();
            POINT pt;
            if (g.shown && g.phase == Phase::kIdle && GetCursorPos(&pt) && OnIsland(g, pt)) {
                g.inside = true;
                Track(g.hwnd);
                if (Island* after = Reach(g)) {
                    SetHover(*after);
                }
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            g.pressed = false;
            EndDrag(g);
            return 0;
        case WM_RBUTTONDOWN:
            return 0;
        case WM_RBUTTONUP: {
            POINT pt;
            if ((wParam & MK_CONTROL) && g.shown && g.phase == Phase::kIdle && !g.dragging &&
                GetCursorPos(&pt) && BodyAt(g, pt) == 0) {
                ToMiddle(g);
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            // Also when the window region goes as the taskbar arrives, with the
            // pointer still on the island.
            POINT pt;
            bool on = GetCursorPos(&pt) && OnIsland(g, pt);
            core::DiagLine(L"Leave %d on %d at %d,%d phase %d", g.id, on, pt.x, pt.y,
                           (int)g.phase);
            g.inside = false;
            KillTimer(g.hwnd, kCrossingTimer);
            if (!on) {
                g.reaching = false;
            }
            SetHover(g);
            return 0;
        }
        // A pointer that came from past the edge and stayed.
        case WM_TIMER: {
            if (wParam != kCrossingTimer) {
                break;
            }
            KillTimer(g.hwnd, kCrossingTimer);
            POINT pt;
            core::DiagLine(L"Crossing %d inside %d", g.id, g.inside);
            if (g.shown && g.phase == Phase::kIdle && g.inside && !g.reaching &&
                !settings::g.island.clickToReveal && !CtrlDown() && GetCursorPos(&pt) &&
                OnIsland(g, pt)) {
                if (Island* after = Reach(g)) {
                    SetHover(*after);
                }
            }
            return 0;
        }
        case WM_DPICHANGED:
            // Layout places it; the suggested rectangle doesn't fit the content.
            g.dpi = LOWORD(wParam);
            g.window = {};
            Sync(g);
            return 0;
        case WM_DISPLAYCHANGE:
            g.window = {};
            Sync(g);
            break;
        default:
            if (msg == ShellHookMsg() &&
                (wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED)) {
                OnActivated(g, (HWND)lParam, wParam == HSHELL_RUDEAPPACTIVATED);
                return 0;
            }
            break;
    }
    return std::nullopt;
}

void Restore();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // A tray's slide can run inside (Expand); unloading waits it out.
    autohide::InFlight counted;
    if (msg == kStopMsg) {
        core::Guard(L"Island stop", [] { Restore(); });
        return 0;
    }
    std::optional<LRESULT> result;
    Island* g = core::g_unloading
                    ? nullptr
                    : FindIf([&](Island const& i) { return i.hwnd == hwnd; });
    if (g) {
        core::Guard(L"Island window", [&] { result = Handle(*g, msg, wParam, lParam); });
    }
    return result ? *result : DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---- drags ----------------------------------------------------------------------
// A drag gets no mouse messages, and the island may cover the hidden
// taskbar's edge strip that brings it out for one (always in click mode). A
// drag coming onto the island brings it out instead, and the taskbar then
// takes the drag. The island itself takes no drop.

void OnDragEnter(int id) {
    Island* p = FindIf([&](Island const& g) { return g.id == id; });
    if (p && p->shown && p->phase == Phase::kIdle) {
        Reach(*p);
    }
}

struct DropTarget : winrt::implements<DropTarget, ::IDropTarget> {
    explicit DropTarget(int id) : id(id) {}

    // Later, on the taskbar thread: the tray's slide doesn't run inside the
    // drag's call.
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject*, DWORD, POINTL, DWORD* effect) noexcept override {
        *effect = DROPEFFECT_NONE;
        core::Guard(L"Island drag", [&] { core::Post([id = id] { OnDragEnter(id); }); });
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL, DWORD* effect) noexcept override {
        *effect = DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject*, DWORD, POINTL, DWORD* effect) noexcept override {
        *effect = DROPEFFECT_NONE;
        return S_OK;
    }

    int id;
};

// ---- lifecycle ------------------------------------------------------------------

bool Register() {
    WNDCLASSEX wc{.cbSize = sizeof(wc),
                  .lpfnWndProc = WndProc,
                  .hInstance = Module(),
                  .hCursor = LoadCursor(nullptr, IDC_ARROW),
                  .lpszClassName = kClassName};
    return RegisterClassEx(&wc) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

void Build(Island& g, HWND taskbar) {
    g.taskbar = taskbar;
    g_thread = GetCurrentThreadId();
    D2D1_FACTORY_OPTIONS options{};
    check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                    __uuidof(ID2D1Factory1), &options,
                                    g.d2d.put_void()));
    check_hresult(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                      (IUnknown**)g.dwrite.put_void()));
    g.wic = winrt::create_instance<IWICImagingFactory>(CLSID_WICImagingFactory);
    g.numberFont = PickFont(g.dwrite.get(), {theme::kClockFont, theme::kIslandFallbackFont});
    g.textFont = PickFont(g.dwrite.get(), {theme::kIslandFallbackFont});
    g.usageFont = PickFont(g.dwrite.get(), {theme::kUsageFont, theme::kIslandFallbackFont});
    CreateDevice(g);

    RECT monitor;
    if (!Register()) {
        Wh_Log(L"Island window class failed");
        return;
    }
    if (!MonitorRect(g, monitor)) {
        Wh_Log(L"Island monitor not found");
        return;
    }
    g.hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        kClassName, L"", WS_POPUP, monitor.left, monitor.top, 1, 1, nullptr, nullptr,
        Module(), nullptr);
    if (!g.hwnd) {
        Wh_Log(L"Island window failed: %u", GetLastError());
        return;
    }
    g.dpi = GetDpiForWindow(g.hwnd);
    if (!g.dpi) {
        g.dpi = 96;
    }
    BOOL on = TRUE;
    DwmSetWindowAttribute(g.hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &on, sizeof(on));
    DWM_WINDOW_CORNER_PREFERENCE corners = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(g.hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners,
                          sizeof(corners));

    g.compositor = wuc::Compositor();
    check_hresult(g.compositor.as<ICompositorDesktopInterop>()->CreateDesktopWindowTarget(
        g.hwnd, TRUE, winrt::put_abi(g.target)));
    g.root = g.compositor.CreateContainerVisual();
    g.root.Opacity(0);
    g.target.Root(g.root);
    check_hresult(g.compositor.as<ICompositorInterop>()->CreateGraphicsDevice(
        g.device.get(), winrt::put_abi(g.graphics)));
    for (auto* surface : {&g.surface, &g.filletSurfaces[0], &g.filletSurfaces[1]}) {
        *surface = g.graphics.CreateDrawingSurface(
            {1, 1}, wgd::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            wgd::DirectXAlphaMode::Premultiplied);
    }
    auto surfaceBrush = [&](wuc::CompositionDrawingSurface const& surface) {
        auto brush = g.compositor.CreateSurfaceBrush(surface);
        brush.Stretch(wuc::CompositionStretch::None);
        return brush;
    };

    g.shapes = g.compositor.CreateContainerVisual();
    g.bodyVisual = g.compositor.CreateShapeVisual();
    g.body = g.compositor.CreateRoundedRectangleGeometry();
    g.fill = g.compositor.CreateColorBrush(IslandFill());
    auto body = g.compositor.CreateSpriteShape(g.body);
    body.FillBrush(g.fill);
    g.bodyVisual.Shapes().Append(body);
    g.shapes.Children().InsertAtTop(g.bodyVisual);
    // The fillets take the body's color through their shapes as masks.
    for (int i = 0; i < 2; i++) {
        auto masked = g.compositor.CreateMaskBrush();
        masked.Source(g.fill);
        masked.Mask(surfaceBrush(g.filletSurfaces[i]));
        g.fillets[i] = g.compositor.CreateSpriteVisual();
        g.fillets[i].Brush(masked);
        g.shapes.Children().InsertAtTop(g.fillets[i]);
    }
    g.content = g.compositor.CreateSpriteVisual();
    g.content.Brush(surfaceBrush(g.surface));
    g.root.Children().InsertAtTop(g.shapes);
    g.root.Children().InsertAtTop(g.content);
    // The corner islands: hidden, at one pixel, until they have something to show.
    for (auto& d : g.sides) {
        d.root = g.compositor.CreateContainerVisual();
        d.root.IsVisible(false);
        d.body = g.compositor.CreateRoundedRectangleGeometry();
        auto shape = g.compositor.CreateSpriteShape(d.body);
        shape.FillBrush(g.fill);
        d.bodyVisual = g.compositor.CreateShapeVisual();
        d.bodyVisual.Shapes().Append(shape);
        d.filletSurface = g.graphics.CreateDrawingSurface(
            {1, 1}, wgd::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            wgd::DirectXAlphaMode::Premultiplied);
        d.surface = g.graphics.CreateDrawingSurface(
            {1, 1}, wgd::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            wgd::DirectXAlphaMode::Premultiplied);
        auto masked = g.compositor.CreateMaskBrush();
        masked.Source(g.fill);
        masked.Mask(surfaceBrush(d.filletSurface));
        d.fillet = g.compositor.CreateSpriteVisual();
        d.fillet.Brush(masked);
        d.content = g.compositor.CreateSpriteVisual();
        d.content.Brush(surfaceBrush(d.surface));
        d.root.Children().InsertAtTop(d.bodyVisual);
        d.root.Children().InsertAtTop(d.fillet);
        d.root.Children().InsertAtTop(d.content);
        g.root.Children().InsertAtTop(d.root);
    }
    g.morphState = g.compositor.CreatePropertySet();
    Bind(g);
    // Tells when a fullscreen app comes and goes.
    RegisterShellHookWindow(g.hwnd);
    // Holds a reference until revoked.
    auto target = winrt::make_self<DropTarget>(g.id);
    HRESULT drop = RegisterDragDrop(g.hwnd, target.as<::IDropTarget>().get());
    g.dropTarget = SUCCEEDED(drop);
    if (!g.dropTarget) {
        Wh_Log(L"Island drop target failed: %08X", (DWORD)drop);
    }
    g.ready = true;
}

// Lets go of all the island holds; the caller drops it from the list.
void Teardown(Island& g) {
    g.ready = false;
    if (g.batch) {
        g.batch.Completed(g.batchDone);
    }
    // The visual is the taskbar's: it gets its opacity back, and stays.
    core::Guard(L"Taskbar reveal", [&] { Unveil(g, false); });
    g.bar = nullptr;
    auto close = [](auto& object) {
        if (object) {
            core::Guard(L"Island release", [&] { object.Close(); });
            object = nullptr;
        }
    };
    close(g.batch);
    close(g.target);
    for (auto& d : g.sides) {
        close(d.content);
        close(d.fillet);
        close(d.bodyVisual);
        close(d.body);
        close(d.root);
        close(d.surface);
        close(d.filletSurface);
        d.on = false;
    }
    close(g.content);
    close(g.fillets[0]);
    close(g.fillets[1]);
    close(g.bodyVisual);
    close(g.body);
    close(g.fill);
    close(g.morphState);
    close(g.reveal);
    close(g.shapes);
    close(g.root);
    close(g.surface);
    close(g.filletSurfaces[0]);
    close(g.filletSurfaces[1]);
    close(g.graphics);
    close(g.compositor);
    if (g.hwnd) {
        if (std::exchange(g.dropTarget, false)) {
            RevokeDragDrop(g.hwnd);
        }
        DeregisterShellHookWindow(g.hwnd);
        DestroyWindow(std::exchange(g.hwnd, nullptr));
    }
}

void NoteWindow() {
    Island* g = FindIf([](Island const&) { return true; });
    g_window = g ? g->hwnd : nullptr;
}

// Islands of taskbars that are gone: a monitor went, or taskbars on all
// displays were turned off.
void Prune() {
    for (auto it = g_islands.begin(); it != g_islands.end();) {
        if (IsWindow(it->taskbar)) {
            ++it;
            continue;
        }
        it->ready = false;
        NoteWindow();
        Teardown(*it);
        it = g_islands.erase(it);
    }
}

// After explorer is done with the window.
void OnTaskbarDestroyed(HWND) {
    core::Post(Prune);
}

// The memory add-on's readings changed: later, once the message is done.
void OnFigures() {
    if (ShowsSystem()) {
        core::Post(SyncAll);
    }
}

// A young explorer may have no device or monitor for a taskbar yet. A taskbar
// whose build failed is tried again as it later goes in or out: a few times in
// all, some seconds apart.
constexpr int kBuildTries = 5;
constexpr ULONGLONG kBuildRetryMs = 2000;
[[clang::no_destroy]] std::vector<HWND> g_unbuilt;  // taskbars whose build failed
int g_buildTries = kBuildTries;
ULONGLONG g_buildFailed;  // tick of the last failure

void Apply(HWND taskbar) {
    if (core::g_vertical || core::g_unloading || !taskbar || Of(taskbar)) {
        return;
    }
    Prune();
    auto it = g_islands.emplace(g_islands.end());
    it->id = ++g_lastId;
    core::Guard(L"Island", [&] { Build(*it, taskbar); });
    if (!it->ready) {
        Teardown(*it);
        g_islands.erase(it);
        g_buildFailed = GetTickCount64();
        if (std::ranges::find(g_unbuilt, taskbar) == g_unbuilt.end()) {
            g_unbuilt.push_back(taskbar);
        }
        if (--g_buildTries > 0) {
            Wh_Log(L"Island tried again later, %d more times", g_buildTries);
            autohide::g_onCollapsedChanged = OnCollapsedChanged;
        }
        return;
    }
    // A late island missed what media sent; it is sent again.
    if (std::erase(g_unbuilt, taskbar)) {
        media::Signal(media::kResend, 0);
    }
    NoteWindow();
    autohide::g_onCollapsedChanged = OnCollapsedChanged;
    autohide::g_revealsAt = RevealsAt;
    autohide::g_onExpandedChanged = OnExpandedChanged;
    autohide::g_standsIn = StandsIn;
    autohide::g_holdsHover = HoldsHover;
    core::g_onTaskbarRestacked = OnTaskbarRestacked;
    core::g_keepsTaskbarTopmost = KeepsTaskbarTopmost;
    core::g_onTaskbarDestroyed = OnTaskbarDestroyed;
    media::g_onShow = OnMedia;
    usage::g_onModel = SyncAll;
    core::g_onFiguresChanged = OnFigures;
    core::Guard(L"Island show", [&] { Sync(*it); });
}

void RetryBuild(HWND taskbar) {
    if (g_buildTries > 0 && std::ranges::find(g_unbuilt, taskbar) != g_unbuilt.end() &&
        GetTickCount64() - g_buildFailed >= kBuildRetryMs) {
        Apply(taskbar);
    }
}

// Taskbar thread, when only the islands' own settings changed: the islands stay
// and take them where they are. The see-through on hover and the fill go to the
// morph's values, the fillets and the content are drawn again, and the layout
// follows the height, width and items.
void Restyle() {
    for (auto& g : g_islands) {
        if (!g.ready) {
            continue;
        }
        core::Guard(L"Island restyle", [&] {
            auto& m = g.morphState;
            m.InsertScalar(L"HoverOpacity", 1 - settings::g.island.hoverTransparency / 100.0f);
            m.InsertColor(L"Black", IslandFill());
            m.InsertColor(L"Pill", PillColor());
            g.filletSize = 0;
            g.shape = {};
            for (auto& d : g.sides) {
                d.filletSize = 0;
                d.shape = {};
            }
            Sync(g);
        });
    }
}

void Restore() {
    autohide::g_onCollapsedChanged = nullptr;
    autohide::g_revealsAt = nullptr;
    autohide::g_onExpandedChanged = nullptr;
    autohide::g_standsIn = nullptr;
    autohide::g_holdsHover = nullptr;
    core::g_onTaskbarRestacked = nullptr;
    core::g_keepsTaskbarTopmost = nullptr;
    core::g_onTaskbarDestroyed = nullptr;
    media::g_onShow = nullptr;
    usage::g_onModel = nullptr;
    core::g_onFiguresChanged = nullptr;
    g_window = nullptr;
    for (auto& g : g_islands) {
        Teardown(g);
    }
    g_islands.clear();
    g_unbuilt.clear();
    g_buildTries = kBuildTries;
    g_thread = 0;
    g_clockText = nullptr;
    g_clock.clear();
    UnregisterClass(kClassName, Module());
    UnwatchPlaces();
}

// Windhawk thread, at unload: the windows go on their own thread even if the
// taskbar can't be reached any more. Any island's window reaches that thread.
// A thread that doesn't answer isn't waited on for long; RestoreAll's run on
// the taskbar thread then takes the windows down.
constexpr UINT kStopWaitMs = 5000;

void Stop() {
    if (HWND hwnd = g_window.exchange(nullptr)) {
        if (!SendMessageTimeout(hwnd, kStopMsg, 0, 0, SMTO_NORMAL | SMTO_ABORTIFHUNG,
                                kStopWaitMs, nullptr)) {
            Wh_Log(L"Island stop got no answer: %u", GetLastError());
        }
    }
}

}  // namespace island

namespace bloom_link {
void Nudge();
}  // namespace bloom_link

// ============================================================================
namespace folders {
// ============================================================================

using Microsoft::WRL::ComPtr;
namespace wadt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Composition;
namespace wux = winrt::Windows::UI::Xaml;

// Pins run `explorer.exe /root,"<folder>"`: the mod catches that command line,
// and without the mod the folder simply opens.
constexpr wchar_t kPinArgPrefix[] = L"/root,\"";
constexpr wchar_t kAppIdPrefix[] = L"TaskbarFolders.";
constexpr wchar_t kPinsDir[] = L".pins";
constexpr wchar_t kOrderFile[] = L".order";
constexpr wchar_t kAppIdsFile[] = L".appids";
constexpr wchar_t kInvalidNameChars[] = L"\\/:*?\"<>|";
constexpr wchar_t kNewFolderName[] = L"폴더";
constexpr wchar_t kTaskListButtonClass[] = L"Taskbar.TaskListButton";
// The click that dismisses a menu also launches its pin; ignore that launch.
constexpr ULONGLONG kToggleMs = 500;
// Share of a button's width on each side that does not count as "on top of it".
constexpr float kMergeEdge = 0.25f;
// shortcut: fixed wait for the taskbar's own async reorder after a drop; wait for a
// pin-list change instead if merges ever read a stale order.
constexpr DWORD kPinSettleMs = 300;
constexpr int kIconCanvas = 256;
// The loose folder: running apps that are neither pinned nor in a folder share one button,
// which the taskbar keeps only while one of them runs. Its shortcut and icon live in
// .pins\.loose; its popup goes by kLooseFolder, which no path can be.
constexpr wchar_t kLooseAppId[] = L"TaskbarFolders.Loose.";
constexpr wchar_t kLooseDir[] = L".loose";
constexpr wchar_t kLooseFolder[] = L"<loose>";
constexpr wchar_t kLooseName[] = L"실행 중";

enum : UINT { kRenameId = 0xFFFE, kIconId = 0xFFFF };
constexpr int kNameEditId = 100;

// PKEY_AppUserModel_ID
constexpr PROPERTYKEY kAppUserModelId = {
    {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
    5};

SRWLOCK g_lock = SRWLOCK_INIT;
std::wstring g_root;        // no trailing backslash; empty disables the mod
std::wstring g_lastFolder;  // folder of the last closed menu
ULONGLONG g_lastCloseTick;
std::vector<HWND> g_menuOwners;  // one per open folder menu; unload closes them
std::atomic<int> g_jobs;         // running worker threads; unload waits for them
std::atomic<bool> g_unloading;
std::atomic<int> g_mergeDelayMs = 500;
// How an open folder and its menu look, from the settings.
struct Look {
    enum class Side { Auto, Above, Below };
    bool labels = true;
    int iconSize = 32, spacing = 4, gap = 8, radius = 20, smoothing = 60;  // DIPs, smoothing %
    int scale = 100;  // % of the whole popup; the gap stays as set
    Side side = Side::Auto;
};
Look g_look;  // under g_lock
bool g_looseOn;            // under g_lock
std::wstring g_looseName;  // under g_lock, a valid file name

double UiScale(const Look& look) {
    return look.scale / 100.0;
}

Look CurrentLook() {
    AcquireSRWLockShared(&g_lock);
    Look look = g_look;
    ReleaseSRWLockShared(&g_lock);
    return look;
}

// Handoff from a merge job to the taskbar animations (under g_lock): the folder button
// about to appear zooms in where the target was, and the target skips its exit.
struct MergeMotion {
    std::wstring enterId, exitId;
    ULONGLONG tick = 0;
};
MergeMotion g_mergeMotion;
std::atomic<bool> g_mergeMotionPending;  // fast path for the animation hooks

void ExpectFolderButton(const std::wstring& enterId, const std::wstring& exitId) {
    AcquireSRWLockExclusive(&g_lock);
    g_mergeMotion = {enterId, exitId, GetTickCount64()};
    ReleaseSRWLockExclusive(&g_lock);
    g_mergeMotionPending = true;
}

std::wstring Root() {
    AcquireSRWLockShared(&g_lock);
    std::wstring root = g_root;
    ReleaseSRWLockShared(&g_lock);
    return root;
}

std::wstring ExpandPath(PCWSTR raw) {
    wchar_t expanded[MAX_PATH], full[MAX_PATH];
    DWORD n = ExpandEnvironmentStringsW(raw, expanded, MAX_PATH);
    if (!n || n > MAX_PATH) return {};
    n = GetFullPathNameW(expanded, MAX_PATH, full, nullptr);
    if (!n || n >= MAX_PATH) return {};
    PathRemoveBackslashW(full);
    return full;
}

// Windows drops a trailing space or dot from a folder name, so those would stop
// matching the path the pin carries.
bool IsValidFolderName(const std::wstring& name) {
    return !name.empty() && name[0] != L'.' && name.back() != L'.' && name.back() != L' ' &&
           name.find_first_of(kInvalidNameChars) == std::wstring::npos;
}

// First of "폴더", "폴더 2", "폴더 3", ... not in `taken`.
std::wstring NextFolderName(const std::vector<std::wstring>& taken) {
    std::wstring name = kNewFolderName;
    for (int n = 2; std::find(taken.begin(), taken.end(), name) != taken.end(); n++)
        name = kNewFolderName + (L" " + std::to_wstring(n));
    return name;
}

std::vector<std::wstring> SplitLines(const std::wstring& text) {
    std::vector<std::wstring> lines;
    for (size_t start = 0; start < text.size();) {
        size_t end = std::min(text.find(L'\n', start), text.size());
        if (end > start) lines.emplace_back(text, start, end - start);
        start = end + 1;
    }
    return lines;
}

// FNV-1a, stable across builds (unlike std::hash).
std::wstring HashHex(std::wstring_view text) {
    uint64_t hash = 14695981039346656037ull;
    for (wchar_t c : text) hash = (hash ^ c) * 1099511628211ull;
    std::wstring hex(16, L'0');
    for (int i = 15; i >= 0; i--, hash >>= 4) hex[i] = L"0123456789abcdef"[hash & 15];
    return hex;
}

std::wstring FolderName(const std::wstring& folder) {
    return PathFindFileNameW(folder.c_str());
}

bool IsLooseId(std::wstring_view appId) {
    constexpr std::wstring_view kPrefix = kLooseAppId;
    return appId.size() >= kPrefix.size() &&
           _wcsnicmp(appId.data(), kPrefix.data(), kPrefix.size()) == 0;
}

bool IsLoose(const std::wstring& folder) {
    return folder == kLooseFolder;
}

std::wstring PinsDir() {
    return Root() + L"\\" + kPinsDir;
}

std::wstring PinLnkPath(const std::wstring& folder) {
    return PinsDir() + L"\\" + FolderName(folder) + L".lnk";
}

// ---- Folder contents ------------------------------------------------------

struct Entry {
    std::wstring name;
    bool isDir;
};

// Visible children; names starting with '.' (.pins, .order) are skipped.
std::vector<Entry> ListEntries(const std::wstring& dir) {
    std::vector<Entry> entries;
    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &fd,
                                   FindExSearchNameMatch, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) return entries;
    do {
        if (fd.cFileName[0] == L'.' ||
            (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)))
            continue;
        entries.push_back(
            {fd.cFileName, (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0});
    } while (FindNextFileW(find, &fd));
    FindClose(find);
    return entries;
}

struct MenuItem {
    std::wstring path;
    std::wstring fileName;
    std::wstring label;
    HBITMAP icon;
};

// Items listed in `order` come first in that order, the rest follow by name.
void SortByOrder(std::vector<MenuItem>& items, const std::vector<std::wstring>& order) {
    auto rank = [&](const MenuItem& item) {
        return std::find(order.begin(), order.end(), item.fileName) - order.begin();
    };
    std::stable_sort(items.begin(), items.end(), [&](const MenuItem& a, const MenuItem& b) {
        auto ra = rank(a), rb = rank(b);
        return ra != rb ? ra < rb : StrCmpLogicalW(a.label.c_str(), b.label.c_str()) < 0;
    });
}

void MoveName(std::vector<std::wstring>& names, size_t from, size_t to) {
    if (from < to)
        std::rotate(names.begin() + from, names.begin() + from + 1, names.begin() + to + 1);
    else
        std::rotate(names.begin() + to, names.begin() + from, names.begin() + from + 1);
}

// Hidden one-entry-per-line files in a folder: .order, .appids.
std::vector<std::wstring> ReadListFile(const std::wstring& dir, PCWSTR name) {
    HANDLE file = CreateFileW((dir + L"\\" + name).c_str(), GENERIC_READ,
                              FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) return {};
    std::wstring text(GetFileSize(file, nullptr) / sizeof(wchar_t), L'\0');
    DWORD read = 0;
    ReadFile(file, text.data(), text.size() * sizeof(wchar_t), &read, nullptr);
    CloseHandle(file);
    text.resize(read / sizeof(wchar_t));
    return SplitLines(text);
}

void WriteListFile(const std::wstring& dir, PCWSTR name, const std::vector<std::wstring>& lines) {
    std::wstring text;
    for (const std::wstring& line : lines) text += line + L"\n";
    // CREATE_ALWAYS on a hidden file fails unless the hidden flag is passed again.
    HANDLE file = CreateFileW((dir + L"\\" + name).c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;
    DWORD written;
    WriteFile(file, text.data(), text.size() * sizeof(wchar_t), &written, nullptr);
    CloseHandle(file);
}

std::vector<std::wstring> ReadOrder(const std::wstring& dir) {
    return ReadListFile(dir, kOrderFile);
}

void WriteOrder(const std::wstring& dir, const std::vector<std::wstring>& names) {
    WriteListFile(dir, kOrderFile, names);
    bloom_link::Nudge();
}

// `.appids` lines are "<file name>\t<app ID>": the taskbar's own ID for an app that
// came from the taskbar, which its windows resolve to.
std::wstring RecordedAppId(const std::wstring& dir, const std::wstring& fileName) {
    for (const std::wstring& line : ReadListFile(dir, kAppIdsFile))
        if (line.size() > fileName.size() && line[fileName.size()] == L'\t' &&
            line.starts_with(fileName))
            return line.substr(fileName.size() + 1);
    return {};
}

void RecordAppId(const std::wstring& dir, const std::wstring& fileName, const std::wstring& appId) {
    std::vector<std::wstring> lines = ReadListFile(dir, kAppIdsFile);
    std::erase_if(lines, [&](const std::wstring& line) { return line.starts_with(fileName + L"\t"); });
    lines.push_back(fileName + L"\t" + appId);
    WriteListFile(dir, kAppIdsFile, lines);
}

void ForgetAppId(const std::wstring& dir, const std::wstring& fileName) {
    std::vector<std::wstring> lines = ReadListFile(dir, kAppIdsFile);
    if (std::erase_if(lines, [&](const std::wstring& line) { return line.starts_with(fileName + L"\t"); }))
        WriteListFile(dir, kAppIdsFile, lines);
}

std::wstring MenuLabel(std::wstring text) {
    for (size_t i = 0; (i = text.find(L'&', i)) != std::wstring::npos; i += 2)
        text.insert(i, 1, L'&');  // a lone & would become a mnemonic
    return text;
}

// Folder contents in menu order; iconSize 0 skips the icons.
std::vector<MenuItem> LoadItems(const std::wstring& dir, int iconSize) {
    std::vector<MenuItem> items;
    for (Entry& entry : ListEntries(dir)) {
        MenuItem item{dir + L"\\" + entry.name, std::move(entry.name), {}, nullptr};
        SHFILEINFOW info{};
        SHGetFileInfoW(item.path.c_str(), 0, &info, sizeof(info), SHGFI_DISPLAYNAME);
        item.label = *info.szDisplayName ? info.szDisplayName : item.fileName;
        ComPtr<IShellItemImageFactory> factory;
        if (iconSize &&
            SUCCEEDED(SHCreateItemFromParsingName(item.path.c_str(), nullptr,
                                                  IID_PPV_ARGS(factory.GetAddressOf()))))
            factory->GetImage({iconSize, iconSize}, SIIGBF_ICONONLY, &item.icon);
        items.push_back(std::move(item));
    }
    SortByOrder(items, ReadOrder(dir));
    return items;
}

std::vector<std::wstring> FileNames(const std::vector<MenuItem>& items) {
    std::vector<std::wstring> names;
    for (const MenuItem& item : items) names.push_back(item.fileName);
    return names;
}

void FreeIcons(const std::vector<MenuItem>& items) {
    for (const MenuItem& item : items) DeleteObject(item.icon);
}

// ---- Shell helpers --------------------------------------------------------

struct PidlFree {
    void operator()(void* pidl) const { CoTaskMemFree(pidl); }
};
using Pidl = std::unique_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>, PidlFree>;

Pidl PidlFromPath(const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    return Pidl(pidl);
}

// Empty for items outside the file system, such as Store apps.
std::wstring PathFromPidl(PCIDLIST_ABSOLUTE pidl) {
    wchar_t path[MAX_PATH];
    return SHGetPathFromIDListW(pidl, path) ? path : L"";
}

ComPtr<IShellLinkW> NewShellLink() {
    ComPtr<IShellLinkW> link;
    CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(link.GetAddressOf()));
    return link;
}

ComPtr<IShellLinkW> LoadShellLink(const std::wstring& path) {
    ComPtr<IShellLinkW> link = NewShellLink();
    ComPtr<IPersistFile> file;
    if (!link || FAILED(link.As(&file)) || FAILED(file->Load(path.c_str(), STGM_READ)))
        return nullptr;
    return link;
}

bool SaveShellLink(const ComPtr<IShellLinkW>& link, const std::wstring& path) {
    ComPtr<IPersistFile> file;
    return SUCCEEDED(link.As(&file)) && SUCCEEDED(file->Save(path.c_str(), TRUE));
}

void Recycle(const std::wstring& path) {
    std::wstring from = path + L'\0';  // the list is double-null terminated
    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = from.c_str();
    op.fFlags = FOF_ALLOWUNDO | FOF_NO_UI;
    SHFileOperationW(&op);
}

void Open(HWND owner, const std::wstring& path) {
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_NOASYNC;  // the calling thread exits right after
    sei.hwnd = owner;
    sei.lpFile = path.c_str();
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

// What to pin for a shortcut in a folder: a Store app's own item, else the shortcut.
Pidl PinnablePidl(const std::wstring& lnk) {
    PIDLIST_ABSOLUTE target = nullptr;
    if (ComPtr<IShellLinkW> link = LoadShellLink(lnk);
        link && SUCCEEDED(link->GetIDList(&target)) && target && PathFromPidl(target).empty())
        return Pidl(target);
    CoTaskMemFree(target);
    return PidlFromPath(lnk);
}

// ---- Pin command line -----------------------------------------------------

std::wstring PinArguments(const std::wstring& folder) {
    return kPinArgPrefix + folder + L"\"";
}

// The folder a pin launch points at, or empty for any other command line.
std::wstring PinnedFolderFromCommand(PCWSTR cmd, const std::wstring& root) {
    if (!cmd || root.empty()) return {};
    std::wstring prefix = kPinArgPrefix + root + L"\\";
    PCWSTR start = StrStrIW(cmd, prefix.c_str());
    if (!start) return {};
    PCWSTR name = start + prefix.size();
    PCWSTR end = wcschr(name, L'"');
    if (!end || end == name || std::find(name, end, L'\\') != end) return {};
    return root + L"\\" + std::wstring(name, end);
}

std::wstring FolderFromLnk(const std::wstring& lnk) {
    wchar_t args[MAX_PATH * 2] = L"";
    ComPtr<IShellLinkW> link = LoadShellLink(lnk);
    if (!link || FAILED(link->GetArguments(args, ARRAYSIZE(args)))) return {};
    return PinnedFolderFromCommand(args, Root());
}

// ---- Pinned list ----------------------------------------------------------
// Undocumented shell interfaces; layouts checked on build 26300.

constexpr CLSID kClsidTaskbandPin = {
    0x90AA3A4E, 0x1CBA, 0x4233, {0xB8, 0xBB, 0x53, 0x57, 0x73, 0xD4, 0x84, 0x49}};
constexpr IID kIidPinnedList3 = {
    0x0DD79AE2, 0xD156, 0x45D4, {0x9E, 0xEB, 0x3B, 0x54, 0x97, 0x69, 0xE9, 0x40}};
constexpr CLSID kClsidPinManager = {
    0xA5C8D635, 0xB4ED, 0x452B, {0x81, 0x09, 0x95, 0x01, 0x78, 0x10, 0x96, 0xD1}};
constexpr IID kIidPinManagerInterop2 = {
    0x87D9E034, 0x56D0, 0x4F8C, {0xBE, 0x59, 0x99, 0x7B, 0x01, 0x75, 0x47, 0x10}};
constexpr IID kIidPinManagerInterop3 = {
    0x27216D91, 0x78B8, 0x406D, {0x85, 0xC4, 0x5F, 0xDB, 0xF5, 0x1C, 0xD1, 0x6B}};

// Only EnumObjects (slot 3) and GetAppIDForPinnedItem (slot 11) are called.
struct IPinnedList3 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE EnumObjects(IEnumFullIDList** items) = 0;
    virtual void STDMETHODCALLTYPE Slot4() = 0;
    virtual void STDMETHODCALLTYPE Slot5() = 0;
    virtual void STDMETHODCALLTYPE Slot6() = 0;
    virtual void STDMETHODCALLTYPE Slot7() = 0;
    virtual void STDMETHODCALLTYPE Slot8() = 0;
    virtual void STDMETHODCALLTYPE Slot9() = 0;
    virtual void STDMETHODCALLTYPE Slot10() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForPinnedItem(PCIDLIST_ABSOLUTE item,
                                                            PWSTR* appId) = 0;
};

enum PINNEDLISTMODIFYCALLER : int {
    PMC_JUMPVIEWBROKER = 11,  // what a native taskbar unpin uses
    PMC_TASKBANDPIN = 25,
    PMC_TASKBANDREORDER = 27,
};

struct IPinManagerInterop2 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE PinItemToTaskbarShim(PCUIDLIST_ABSOLUTE, PINNEDLISTMODIFYCALLER) = 0;
    virtual HRESULT STDMETHODCALLTYPE PinItemFromTrustedCaller(PCUIDLIST_ABSOLUTE pidl,
                                                               PINNEDLISTMODIFYCALLER caller) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyPrependDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyAppendDefaultTaskbarLayout() = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyInPlaceTaskbarLayout(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE ApplyReorderTaskbarLayout(int, int) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnpinTaskbarItem(PCUIDLIST_ABSOLUTE pidl,
                                                       PINNEDLISTMODIFYCALLER caller) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdatePinnedTaskbarItem(PCUIDLIST_ABSOLUTE, PCUIDLIST_ABSOLUTE,
                                                              PINNEDLISTMODIFYCALLER) = 0;
};

struct IPinManagerInterop3 : IPinManagerInterop2 {
    virtual HRESULT STDMETHODCALLTYPE MoveTaskbarPin(PCUIDLIST_ABSOLUTE pidl, int index,
                                                     PINNEDLISTMODIFYCALLER caller) = 0;
};

struct Pin {
    std::wstring appId;
    Pidl pidl;
};

// Pinned items in taskbar order.
std::vector<Pin> ReadPins() {
    std::vector<Pin> pins;
    ComPtr<IPinnedList3> list;
    ComPtr<IEnumFullIDList> items;
    if (FAILED(CoCreateInstance(kClsidTaskbandPin, nullptr, CLSCTX_INPROC_SERVER,
                                kIidPinnedList3, reinterpret_cast<void**>(list.GetAddressOf()))) ||
        FAILED(list->EnumObjects(items.GetAddressOf())))
        return pins;
    for (PIDLIST_ABSOLUTE pidl; items->Next(1, &pidl, nullptr) == S_OK;) {
        Pin pin{{}, Pidl(pidl)};
        PWSTR appId = nullptr;
        if (SUCCEEDED(list->GetAppIDForPinnedItem(pidl, &appId)) && appId) {
            pin.appId = appId;
            CoTaskMemFree(appId);
        }
        pins.push_back(std::move(pin));
    }
    return pins;
}

int FindPin(const std::vector<Pin>& pins, const std::wstring& appId) {
    for (size_t i = 0; i < pins.size(); i++)
        if (_wcsicmp(pins[i].appId.c_str(), appId.c_str()) == 0) return i;
    return -1;
}

// Where a drop between buttons pins: just after the last pinned app of `leftIds` (the
// buttons left of the drop; running apps among them have no pin), or first when none is.
int PinIndexAfter(const std::vector<Pin>& pins, const std::vector<std::wstring>& leftIds) {
    int index = 0;
    for (const std::wstring& id : leftIds)
        if (int pin = FindPin(pins, id); pin >= 0) index = std::max(index, pin + 1);
    return index;
}

std::wstring FolderFromAppId(const std::wstring& appId);

// The folder behind a pinned folder, or empty for an app. A pin whose shortcut file is
// gone is still known by its app ID, which follows from the folder's path.
std::wstring FolderOfPin(const Pin& pin) {
    if (!pin.appId.starts_with(kAppIdPrefix)) return L"";
    std::wstring folder = FolderFromLnk(PathFromPidl(pin.pidl.get()));
    return folder.empty() ? FolderFromAppId(pin.appId) : folder;
}

int FindFolderPin(const std::vector<Pin>& pins, const std::wstring& folder) {
    for (size_t i = 0; i < pins.size(); i++)
        if (_wcsicmp(FolderOfPin(pins[i]).c_str(), folder.c_str()) == 0) return i;
    return -1;
}

ComPtr<IPinManagerInterop3> PinManager() {
    ComPtr<IPinManagerInterop2> v2;
    ComPtr<IPinManagerInterop3> v3;
    if (SUCCEEDED(CoCreateInstance(kClsidPinManager, nullptr,
                                   CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                                   kIidPinManagerInterop2,
                                   reinterpret_cast<void**>(v2.GetAddressOf()))))
        v2->QueryInterface(kIidPinManagerInterop3, reinterpret_cast<void**>(v3.GetAddressOf()));
    return v3;
}

// Pins `item` and moves it to `index`; new pins land at the end first.
bool PinAt(IPinManagerInterop3* manager, PCIDLIST_ABSOLUTE item, int index) {
    size_t before = ReadPins().size();
    HRESULT hr = manager->PinItemFromTrustedCaller(item, PMC_TASKBANDPIN);
    std::vector<Pin> after = ReadPins();
    bool pinned = SUCCEEDED(hr) && after.size() > before;
    if (pinned) manager->MoveTaskbarPin(after.back().pidl.get(), index, PMC_TASKBANDREORDER);
    Wh_Log(L"pin at %d: hr=%08X pinned=%d", index, hr, pinned);
    return pinned;
}

void Unpin(IPinManagerInterop3* manager, PCIDLIST_ABSOLUTE item) {
    HRESULT hr = manager->UnpinTaskbarItem(item, PMC_JUMPVIEWBROKER);
    Wh_Log(L"unpin: hr=%08X", hr);
}

// Puts `item` where pins[index] is; the old pin goes only once the new one is in.
bool ReplacePin(IPinManagerInterop3* manager, const std::vector<Pin>& pins, int index,
                PCIDLIST_ABSOLUTE item) {
    if (!PinAt(manager, item, index)) return false;
    Unpin(manager, pins[index].pidl.get());
    return true;
}

// ---- Folder icon ----------------------------------------------------------

using Pixels = std::vector<uint32_t>;  // premultiplied BGRA, top-down

// A colour channel above alpha cannot be premultiplied.
bool StraightAlpha(uint32_t p) {
    uint32_t a = p >> 24;
    return ((p >> 16) & 255) > a || ((p >> 8) & 255) > a || (p & 255) > a;
}

uint32_t Premultiply(uint32_t p) {
    uint32_t a = p >> 24, out = a << 24;
    for (int shift = 0; shift < 24; shift += 8) out |= (((p >> shift) & 255) * a + 127) / 255 << shift;
    return out;
}

bool IconPixels(IShellItem* item, int size, Pixels* pixels) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (!item || FAILED(item->QueryInterface(IID_PPV_ARGS(factory.GetAddressOf()))) ||
        FAILED(factory->GetImage({size, size}, SIIGBF_ICONONLY, &bitmap)))
        return false;
    BITMAPINFO info{{sizeof(BITMAPINFOHEADER), size, -size, 1, 32, BI_RGB}};
    pixels->assign(size * size, 0);
    HDC dc = GetDC(nullptr);
    bool ok = GetDIBits(dc, bitmap, 0, size, pixels->data(), &info, DIB_RGB_COLORS) == size;
    ReleaseDC(nullptr, dc);
    DeleteObject(bitmap);
    if (ok && std::none_of(pixels->begin(), pixels->end(), [](uint32_t p) { return p >> 24; }))
        for (uint32_t& p : *pixels) p |= 0xFF000000;  // old icons without alpha
    // The shell hands most icons over with straight alpha; drawn as premultiplied, their
    // soft edges come out as a light fringe.
    else if (ok && std::any_of(pixels->begin(), pixels->end(), StraightAlpha))
        for (uint32_t& p : *pixels) p = Premultiply(p);
    return ok;
}

bool IconPixels(const std::wstring& path, int size, Pixels* pixels) {
    ComPtr<IShellItem> item;
    return SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                                 IID_PPV_ARGS(item.GetAddressOf()))) &&
           IconPixels(item.Get(), size, pixels);
}

// Premultiplied "src over dst".
uint32_t Over(uint32_t src, uint32_t dst) {
    uint32_t keep = 255 - (src >> 24), out = 0;
    for (int shift = 0; shift < 32; shift += 8)
        out |= (((src >> shift) & 255) + (((dst >> shift) & 255) * keep + 127) / 255) << shift;
    return out;
}

// Antialiased coverage of a rounded square inset from the canvas edges.
float RoundRectCoverage(float x, float y, float size, float inset, float radius) {
    float half = size / 2 - inset - radius;
    float dx = std::fabs(x - size / 2) - half, dy = std::fabs(y - size / 2) - half;
    float distance = std::hypot(std::max(dx, 0.f), std::max(dy, 0.f)) +
                     std::min(std::max(dx, dy), 0.f) - radius;
    return std::clamp(0.5f - distance, 0.f, 1.f);
}

// The first four apps on a translucent tile, like a phone folder.
Pixels FolderPreview(const std::vector<MenuItem>& items) {
    constexpr int kPad = 28, kIcon = 92, kGap = 16;
    Pixels canvas(kIconCanvas * kIconCanvas, 0);
    for (int y = 0; y < kIconCanvas; y++)
        for (int x = 0; x < kIconCanvas; x++) {
            float c = RoundRectCoverage(x + 0.5f, y + 0.5f, kIconCanvas, 6, 58);
            // (160, 166, 172) at 42%: visible on both light and dark taskbars.
            auto channel = [c](float v) { return uint32_t(v * 0.42f * c + 0.5f); };
            canvas[y * kIconCanvas + x] = channel(255) << 24 | channel(160) << 16 |
                                          channel(166) << 8 | channel(172);
        }
    Pixels icon;
    for (size_t i = 0; i < std::min<size_t>(items.size(), 4); i++) {
        if (!IconPixels(items[i].path, kIcon, &icon)) continue;
        int left = kPad + int(i % 2) * (kIcon + kGap), top = kPad + int(i / 2) * (kIcon + kGap);
        for (int y = 0; y < kIcon; y++)
            for (int x = 0; x < kIcon; x++) {
                uint32_t& dst = canvas[(top + y) * kIconCanvas + left + x];
                dst = Over(icon[y * kIcon + x], dst);
            }
    }
    return canvas;
}

// Distance from (x, y) to the segment from the origin to (ex, ey).
float SegmentDistance(float x, float y, float ex, float ey) {
    float t = std::clamp((x * ex + y * ey) / (ex * ex + ey * ey), 0.f, 1.f);
    return std::hypot(x - ex * t, y - ey * t);
}

// The loose folder's icon, drawn as the folder icon sets are: a squircle tile with a
// diagonal gradient and one flat glyph, here a clock at three. The silver (`light`) or
// the graphite tile, as the sets come in both.
Pixels ClockIcon(bool light) {
    constexpr float kCenter = kIconCanvas / 2.f;
    constexpr float kTile = 102, kTileCurve = 5.9f;  // the sets' tile: 204 of 256, n = 5.9
    constexpr float kRing = 48.5f, kStroke = 15, kHand = 14;
    constexpr float kLight[3][3] = {{0xF7, 0xF7, 0xF9}, {0xBA, 0xBA, 0xC0}, {0x1C, 0x1C, 0x1F}};
    constexpr float kDark[3][3] = {{0x5E, 0x5E, 0x64}, {0x3C, 0x3C, 0x42}, {0xFF, 0xFF, 0xFF}};
    const auto& [top, bottom, glyph] = light ? kLight : kDark;
    Pixels canvas(kIconCanvas * kIconCanvas, 0);
    for (int y = 0; y < kIconCanvas; y++)
        for (int x = 0; x < kIconCanvas; x++) {
            float px = x + 0.5f - kCenter, py = y + 0.5f - kCenter;
            // A superellipse's own measure is close enough to its distance at the edge.
            float edge = std::pow(std::pow(std::fabs(px), kTileCurve) +
                                      std::pow(std::fabs(py), kTileCurve),
                                  1 / kTileCurve) -
                         kTile;
            float tile = std::clamp(0.5f - edge, 0.f, 1.f);
            if (tile <= 0) continue;
            float ring = std::fabs(std::hypot(px, py) - kRing) - kStroke / 2;
            float hands = std::min(SegmentDistance(px, py, 0, -27),
                                   SegmentDistance(px, py, 20, 0)) -
                          kHand / 2;
            float mark = std::clamp(0.5f - std::min(ring, hands), 0.f, 1.f);
            float t = std::clamp((px + py + 2 * kTile) / (4 * kTile), 0.f, 1.f);
            auto channel = [&](int i) {
                float fill = top[i] + (bottom[i] - top[i]) * t;
                return uint32_t((fill + (glyph[i] - fill) * mark) * tile + 0.5f);
            };
            canvas[y * kIconCanvas + x] = uint32_t(tile * 255 + 0.5f) << 24 | channel(0) << 16 |
                                          channel(1) << 8 | channel(2);
        }
    return canvas;
}

// The canvas encoded as PNG.
std::vector<BYTE> EncodePng(IWICImagingFactory* wic, IWICBitmapSource* source) {
    ComPtr<IStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    UINT width = 0, height = 0;
    STATSTG stat{};
    if (FAILED(source->GetSize(&width, &height)) ||
        FAILED(CreateStreamOnHGlobal(nullptr, TRUE, stream.GetAddressOf())) ||
        FAILED(wic->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.GetAddressOf())) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(frame.GetAddressOf(), nullptr)) ||
        FAILED(frame->Initialize(nullptr)) || FAILED(frame->SetSize(width, height)) ||
        FAILED(frame->SetPixelFormat(&format)) || FAILED(frame->WriteSource(source, nullptr)) ||
        FAILED(frame->Commit()) || FAILED(encoder->Commit()) ||
        FAILED(stream->Stat(&stat, STATFLAG_NONAME)))
        return {};
    std::vector<BYTE> png(stat.cbSize.LowPart);
    ULONG read = 0;
    stream->Seek({}, STREAM_SEEK_SET, nullptr);
    stream->Read(png.data(), png.size(), &read);
    return read == png.size() ? png : std::vector<BYTE>{};
}

// The canvas scaled to `size` as a classic 32-bit icon image (DIB plus an empty mask),
// the form the shell reads for small icons.
std::vector<BYTE> EncodeDib(IWICImagingFactory* wic, IWICBitmapSource* source, int size) {
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICFormatConverter> converter;
    int row = size * 4, mask = (size + 31) / 32 * 4 * size;
    std::vector<BYTE> pixels(row * size);
    if (FAILED(wic->CreateBitmapScaler(scaler.GetAddressOf())) ||
        FAILED(scaler->Initialize(source, size, size, WICBitmapInterpolationModeFant)) ||
        FAILED(wic->CreateFormatConverter(converter.GetAddressOf())) ||
        FAILED(converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppBGRA,
                                     WICBitmapDitherTypeNone, nullptr, 0,
                                     WICBitmapPaletteTypeCustom)) ||
        FAILED(converter->CopyPixels(nullptr, row, pixels.size(), pixels.data())))
        return {};
    BITMAPINFOHEADER header{sizeof(header), size, size * 2, 1, 32, BI_RGB};
    std::vector<BYTE> dib(sizeof(header) + pixels.size() + mask);
    memcpy(dib.data(), &header, sizeof(header));
    for (int y = 0; y < size; y++)  // DIB rows run bottom-up
        memcpy(&dib[sizeof(header) + y * row], &pixels[(size - 1 - y) * row], row);
    return dib;
}

// An .ico with the canvas at the shell's small sizes plus a 256px PNG.
std::vector<BYTE> EncodeIco(const Pixels& pixels, int size) {
    constexpr int kSizes[] = {16, 20, 24, 32, 40, 48, 64};
    ComPtr<IWICImagingFactory> wic;
    ComPtr<IWICBitmap> bitmap;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(wic.GetAddressOf()))) ||
        FAILED(wic->CreateBitmapFromMemory(
            size, size, GUID_WICPixelFormat32bppPBGRA, size * 4, size * size * 4,
            reinterpret_cast<BYTE*>(const_cast<uint32_t*>(pixels.data())),
            bitmap.GetAddressOf())))
        return {};
    std::vector<std::pair<int, std::vector<BYTE>>> images;
    for (int s : kSizes) images.emplace_back(s, EncodeDib(wic.Get(), bitmap.Get(), s));
    images.emplace_back(size, EncodePng(wic.Get(), bitmap.Get()));

    // ICONDIR, one 16-byte ICONDIRENTRY per image, then the images.
    std::vector<BYTE> ico(6 + 16 * images.size());
    const WORD dir[] = {0, 1, WORD(images.size())};
    memcpy(&ico[0], dir, 6);
    for (size_t i = 0; i < images.size(); i++) {
        const auto& [s, data] = images[i];
        if (data.empty()) return {};
        BYTE* entry = &ico[6 + 16 * i];
        entry[0] = entry[1] = BYTE(s < 256 ? s : 0);  // 0 means 256
        const WORD planes = 1, bits = 32;
        const DWORD bytes = data.size(), offset = ico.size();
        memcpy(entry + 4, &planes, 2);
        memcpy(entry + 6, &bits, 2);
        memcpy(entry + 8, &bytes, 4);
        memcpy(entry + 12, &offset, 4);
        ico.insert(ico.end(), data.begin(), data.end());
    }
    return ico;
}

bool WriteFileBytes(const std::wstring& path, const std::vector<BYTE>& bytes) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = WriteFile(file, bytes.data(), bytes.size(), &written, nullptr) &&
              written == bytes.size();
    CloseHandle(file);
    return ok;
}

// Preview icons are `.pins\<folder>.<hash>.ico`; a new name makes the taskbar reload it.
std::wstring PreviewIconPath(const std::wstring& folder, const std::vector<MenuItem>& items) {
    std::wstring key = L"ico2\n";  // the file format: a new one never reuses old files
    for (size_t i = 0; i < std::min<size_t>(items.size(), 4); i++) key += items[i].path + L"\n";
    return PinsDir() + L"\\" + FolderName(folder) + L"." + HashHex(key) + L".ico";
}

// Exactly `<name>.<16 hex>.ico`, so folder "A" never claims the icons of "A.B".
bool IsPreviewIconOf(const std::wstring& fileName, const std::wstring& name) {
    size_t n = name.size(), hash = n + 1;
    return fileName.size() == hash + 16 + 4 && _wcsnicmp(fileName.c_str(), name.c_str(), n) == 0 &&
           fileName[n] == L'.' && _wcsicmp(fileName.c_str() + hash + 16, L".ico") == 0 &&
           std::all_of(fileName.begin() + hash, fileName.begin() + hash + 16, iswxdigit);
}

void DeletePreviewIcons(const std::wstring& folder, const std::wstring& keep) {
    std::wstring name = FolderName(folder), pattern = PinsDir() + L"\\" + name + L".*.ico";
    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileW(pattern.c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring path = PinsDir() + L"\\" + fd.cFileName;
        if (IsPreviewIconOf(fd.cFileName, name) && _wcsicmp(path.c_str(), keep.c_str()) != 0)
            DeleteFileW(path.c_str());
    } while (FindNextFileW(find, &fd));
    FindClose(find);
}

// Old previews stay until the pin using the new one is in place.
std::wstring MakePreviewIcon(const std::wstring& folder) {
    std::vector<MenuItem> items = LoadItems(folder, 0);
    std::wstring path = PreviewIconPath(folder, items);
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES &&
        !WriteFileBytes(path, EncodeIco(FolderPreview(items), kIconCanvas)))
        return {};
    return path;
}

struct IconLocation {
    std::wstring path;
    int index = 0;
};

IconLocation PinIcon(const std::wstring& folder) {
    IconLocation icon;
    wchar_t path[MAX_PATH] = L"";
    if (ComPtr<IShellLinkW> link = LoadShellLink(PinLnkPath(folder));
        link && SUCCEEDED(link->GetIconLocation(path, MAX_PATH, &icon.index)))
        icon.path = path;
    return icon;
}

// Previews live in .pins; folders from 1.x still carry the stock folder icon. Other
// icons from the same file are the user's picks.
bool IsPreviewIcon(const IconLocation& icon) {
    SHSTOCKICONINFO stock{sizeof(stock)};
    return icon.path.empty() ||
           (StrStrIW(icon.path.c_str(), (PinsDir() + L"\\").c_str()) &&
            !StrStrIW(icon.path.c_str(), (PinsDir() + L"\\.icons\\").c_str())) ||
           (SUCCEEDED(SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICONLOCATION, &stock)) &&
            _wcsicmp(ExpandPath(icon.path.c_str()).c_str(), ExpandPath(stock.szPath).c_str()) == 0 &&
            icon.index == stock.iIcon);
}

// ---- Folder pins ----------------------------------------------------------

// An own app ID keeps a folder pin from merging into File Explorer's button.
std::wstring FolderAppId(const std::wstring& folder) {
    return kAppIdPrefix + HashHex(folder);
}

// A shortcut at `path` to explorer.exe with `arguments`, under its own app ID.
bool WriteExplorerLnk(const std::wstring& path, const std::wstring& arguments,
                      const IconLocation& icon, std::wstring appId) {
    ComPtr<IShellLinkW> link = NewShellLink();
    if (!link) return false;
    wchar_t explorer[MAX_PATH];
    GetWindowsDirectoryW(explorer, MAX_PATH);
    PathAppendW(explorer, L"explorer.exe");
    link->SetPath(explorer);
    link->SetArguments(arguments.c_str());
    link->SetIconLocation(icon.path.c_str(), icon.index);

    PROPVARIANT value{};
    value.vt = VT_LPWSTR;
    value.pwszVal = appId.data();
    ComPtr<IPropertyStore> store;
    return SUCCEEDED(link.As(&store)) && SUCCEEDED(store->SetValue(kAppUserModelId, value)) &&
           SUCCEEDED(store->Commit()) && SaveShellLink(link, path);
}

bool WritePinLnk(const std::wstring& folder, const IconLocation& icon) {
    return WriteExplorerLnk(PinLnkPath(folder), PinArguments(folder), icon, FolderAppId(folder));
}

// Pins the folder under its new path in place of `pinnedFolder`'s pin (a rename: the
// app ID changes with the path). On failure the new `.pins` link goes again.
bool RepinFolder(IPinManagerInterop3* manager, const std::wstring& pinnedFolder,
                 const std::wstring& folder, const IconLocation& icon) {
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, pinnedFolder);
    if (index < 0) return false;
    if (WritePinLnk(folder, icon) &&
        ReplacePin(manager, pins, index, PidlFromPath(PinLnkPath(folder)).get()))
        return true;
    DeleteFileW(PinLnkPath(folder).c_str());
    return false;
}

// Tells the shell a shortcut file changed, at once (a notice left queued may never go out).
void NotifyUpdated(const std::wstring& path) {
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path.c_str(), nullptr);
}

// A folder pin whose shortcut file went missing shows a blank icon and takes no new one;
// the folder's own copy in .pins puts it back.
void RestorePinLink(const Pin& pin, const std::wstring& folder) {
    std::wstring pinned = PathFromPidl(pin.pidl.get());
    if (pinned.empty() || GetFileAttributesW(pinned.c_str()) != INVALID_FILE_ATTRIBUTES) return;
    bool ok = CopyFileW(PinLnkPath(folder).c_str(), pinned.c_str(), TRUE);
    if (ok) SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, pinned.c_str(), nullptr);
    Wh_Log(L"pin shortcut of %s restored: %d", folder.c_str(), ok);
}

std::vector<std::wstring> FolderKeys(const std::wstring& folder);
void Regroup(const std::vector<std::wstring>& keys);
void RegroupLoose(bool all);
extern SRWLOCK g_looseWork;

// The taskbar reads a pinned shortcut's icon as it is pinned and not again, whatever the
// shell is told about the file: the folder is pinned anew in its own place. One pin per
// app ID, so the old pin goes first, and the new one gets a second try.
// `looseLast`: the loose folder goes to the end after it; one doing several folders sends
// it there once, after the last.
void RepinInPlace(const std::wstring& folder, bool looseLast = true) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (!manager || index < 0 || g_unloading) return;
    Unpin(manager.Get(), pins[index].pidl.get());
    Sleep(kPinSettleMs);
    bool ok = PinAt(manager.Get(), PidlFromPath(PinLnkPath(folder)).get(), index);
    if (!ok) ok = PinAt(manager.Get(), PidlFromPath(PinLnkPath(folder)).get(), index);
    Wh_Log(L"repin %s at %d: %d", folder.c_str(), index, ok);
    Regroup(FolderKeys(folder));
    if (!looseLast) return;
    // A new pin's button comes after the buttons there are, the loose folder's too:
    // that one goes again, to the end.
    AcquireSRWLockExclusive(&g_looseWork);
    RegroupLoose(true);
    ReleaseSRWLockExclusive(&g_looseWork);
}

// The taskbar keeps one pin per app ID, so a new icon goes into the pinned shortcut
// itself, and the pin is made again for the taskbar to show it.
bool SetFolderIcon(const std::wstring& folder, const IconLocation& icon, bool looseLast = true) {
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (index >= 0) RestorePinLink(pins[index], folder);
    std::wstring pinned = index >= 0 ? PathFromPidl(pins[index].pidl.get()) : L"";
    ComPtr<IShellLinkW> link = pinned.empty() ? nullptr : LoadShellLink(pinned);
    bool ok = link && SUCCEEDED(link->SetIconLocation(icon.path.c_str(), icon.index)) &&
              SaveShellLink(link, pinned) && WritePinLnk(folder, icon);
    if (ok) RepinInPlace(folder, looseLast);
    Wh_Log(L"icon of %s: %d", folder.c_str(), ok);
    if (ok) bloom_link::Nudge();
    return ok;
}

void RefreshPreview(const std::wstring& folder) {
    IconLocation icon = PinIcon(folder);
    if (!IsPreviewIcon(icon)) return;  // the user picked one
    IconLocation preview{MakePreviewIcon(folder)};
    if (!preview.path.empty() && _wcsicmp(preview.path.c_str(), icon.path.c_str()) != 0 &&
        SetFolderIcon(folder, preview))
        DeletePreviewIcons(folder, preview.path);
}

// The folder's item `name` goes last in its order.
void PutLast(const std::wstring& folder, const std::wstring& name) {
    std::vector<std::wstring> order = FileNames(LoadItems(folder, 0));
    std::erase(order, name);
    order.push_back(name);
    WriteOrder(folder, order);
}

std::wstring FreeName(const std::wstring& dir, const std::wstring& fileName);

// Puts a shortcut to `item` into the folder, at the end of its order: a shortcut file is
// copied, anything else (a program, a Store app) gets a new one, named as the item without
// its extension. A taken name gets a number when `numbered`. Returns its file name; empty
// if that failed or the name is taken.
std::wstring AddShortcutInto(PCIDLIST_ABSOLUTE item, const std::wstring& folder,
                             const std::wstring& appId = L"", bool numbered = false) {
    std::wstring path = PathFromPidl(item), name;
    PCWSTR extension = PathFindExtensionW(path.c_str());
    bool copy = _wcsicmp(extension, L".lnk") == 0 || _wcsicmp(extension, L".url") == 0;
    if (copy) {
        name = PathFindFileNameW(path.c_str());
    } else {
        if (!path.empty()) {
            name = PathFindFileNameW(path.c_str());
            if (!PathIsDirectoryW(path.c_str())) name.resize(name.size() - wcslen(extension));
        } else {
            PWSTR display = nullptr;
            if (FAILED(SHGetNameFromIDList(item, SIGDN_NORMALDISPLAY, &display))) return {};
            name = display;
            CoTaskMemFree(display);
        }
        std::replace_if(name.begin(), name.end(),
                        [](wchar_t c) { return wcschr(kInvalidNameChars, c) != nullptr; }, L'_');
        name += L".lnk";
    }
    if (numbered) name = FreeName(folder, name);
    if (name.empty()) return {};
    std::wstring target = folder + L"\\" + name;
    bool ok = GetFileAttributesW(target.c_str()) == INVALID_FILE_ATTRIBUTES;
    if (ok && copy) {
        ok = CopyFileW(path.c_str(), target.c_str(), TRUE);
    } else if (ok) {
        ComPtr<IShellLinkW> link = NewShellLink();
        ok = link && SUCCEEDED(link->SetIDList(item)) && SaveShellLink(link, target);
    }
    if (ok) {
        PutLast(folder, name);
        if (!appId.empty()) RecordAppId(folder, name, appId);
    }
    Wh_Log(L"add %s into %s: %d", name.c_str(), folder.c_str(), ok);
    return ok ? name : L"";
}

// Copies a pinned app's shortcut into the folder.
bool CopyPinInto(const Pin& pin, const std::wstring& folder) {
    return !AddShortcutInto(pin.pidl.get(), folder, pin.appId).empty();
}

void RemoveFolder(const std::wstring& folder) {
    DeleteFileW(PinLnkPath(folder).c_str());
    DeletePreviewIcons(folder, L"");
    Recycle(folder);
}

// ---- Running apps (Taskbar.dll) ----------------------------------------------

// A folder app's windows join the folder's own taskbar group. The taskbar resolves which
// app a window belongs to (CTaskBand::_HandleWindowResolved, then _HandleItemResolved),
// and the mod answers with the folder's app ID, so no second button appears and the
// folder shows running, active and preview states the Windows way.

std::wstring Upper(std::wstring text) {
    CharUpperBuffW(text.data(), text.size());
    return text;
}

// "{known folder}\rest", the taskbar's form of a desktop app's path, as a plain path.
std::wstring ExpandAppIdPath(const std::wstring& appId) {
    size_t close = appId.find(L"}\\");
    GUID id;
    PWSTR base = nullptr;
    if (appId.empty() || appId[0] != L'{' || close == std::wstring::npos ||
        FAILED(CLSIDFromString(appId.substr(0, close + 1).c_str(), &id)) ||
        FAILED(SHGetKnownFolderPath(id, KF_FLAG_DONT_VERIFY, nullptr, &base)))
        return appId;
    std::wstring path = base + appId.substr(close + 1);
    CoTaskMemFree(base);
    return path;
}

// The names the taskbar may know an app by, in one comparable form.
std::wstring AppKey(const std::wstring& appIdOrPath) {
    return Upper(ExpandAppIdPath(appIdOrPath));
}

// The app ID of a shortcut to an Apps folder item (a Store or packaged app), which is the
// item's name there; empty for a shortcut to a file.
std::wstring AppsFolderId(IShellLinkW* link) {
    wchar_t target[MAX_PATH] = L"";
    PIDLIST_ABSOLUTE item = nullptr;
    PWSTR name = nullptr;
    std::wstring id;
    if (link->GetPath(target, MAX_PATH, nullptr, 0) != S_OK && SUCCEEDED(link->GetIDList(&item)) &&
        item && SUCCEEDED(SHGetNameFromIDList(item, SIGDN_PARENTRELATIVEPARSING, &name))) {
        id = name;
        CoTaskMemFree(name);
    }
    CoTaskMemFree(item);
    return id;
}

// Every key a folder item's app may resolve to: the taskbar's ID from when it was
// pinned, the shortcut's target (a program, or a Store app whose name in the Apps
// folder is its app ID), and an app ID the shortcut sets.
std::vector<std::wstring> ItemKeys(const std::wstring& dir, const std::wstring& fileName) {
    std::vector<std::wstring> keys;
    auto add = [&](const std::wstring& key) {
        if (!key.empty()) keys.push_back(AppKey(key));
    };
    add(RecordedAppId(dir, fileName));
    if (ComPtr<IShellLinkW> link = LoadShellLink(dir + L"\\" + fileName)) {
        wchar_t target[MAX_PATH] = L"";
        if (link->GetPath(target, MAX_PATH, nullptr, 0) == S_OK)
            add(target);
        else
            add(AppsFolderId(link.Get()));
        ComPtr<IPropertyStore> store;
        PROPVARIANT value{};
        if (SUCCEEDED(link.As(&store)) && SUCCEEDED(store->GetValue(kAppUserModelId, &value))) {
            if (value.vt == VT_LPWSTR) add(value.pwszVal);
            PropVariantClear(&value);
        }
    }
    return keys;
}

std::vector<std::wstring> FolderKeys(const std::wstring& folder) {
    std::vector<std::wstring> keys;
    for (const MenuItem& item : LoadItems(folder, 0))
        for (std::wstring& key : ItemKeys(folder, item.fileName)) keys.push_back(std::move(key));
    return keys;
}

// A pinned folder as the taskbar knows its button: the pin's app ID and shortcut.
struct FolderPin {
    std::wstring appId;
    std::shared_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>> shortcut;
};

// A window in the loose folder, with what the taskbar resolved for it before it joined:
// its own app ID, program and app item, which the folder's popup shows and starts.
struct LooseWindow {
    std::wstring key, appId, path;
    std::shared_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>> item;
    uint64_t order = 0;  // when it came, for the popup's order
};

// Under g_lock. `members`: app key -> its folder's pin, rebuilt on workers after changes;
// `windows`: window the hook grouped into a folder -> its app key.
struct Running {
    std::unordered_map<std::wstring, FolderPin> members;
    std::unordered_map<HWND, std::wstring> windows;
    // Pinned apps' keys, rebuilt with `members`: their windows keep their own buttons.
    std::unordered_set<std::wstring> pinned;
    // The loose folder's button (empty while it is off) and its windows, which are in
    // `windows` too.
    FolderPin looseFolder;
    std::unordered_map<HWND, LooseWindow> loose;
    uint64_t looseOrder = 0;
};
Running g_running;
std::atomic<bool> g_haveMembers;  // fast path for the hook
std::atomic<bool> g_pinsKnown;    // `pinned` was read at least once
std::atomic<bool> g_looseReady;   // the hook may send windows to the loose folder

// Set on the windows a copy of the mod lets go of on unload; the next copy takes them back.
constexpr wchar_t kReleasedProp[] = L"TaskbarFolders.Released";
// Set on explorer's taskbar window by a copy that let windows go: when (GetTickCount64).
constexpr wchar_t kReleasedAtProp[] = L"TaskbarFolders.ReleasedAt";

// Only pinned folders take windows, under their button's own ID: a folder left on disk
// would take them into a group with no button. An app in two folders goes to the first.
void RebuildMembers() {
    std::unordered_map<std::wstring, FolderPin> members;
    std::unordered_set<std::wstring> pinned;
    std::vector<Pin> pins = ReadPins();
    for (const Pin& pin : pins) {
        std::wstring folder = FolderOfPin(pin);
        if (folder.empty()) {
            if (!pin.appId.empty()) pinned.insert(AppKey(pin.appId));
            continue;
        }
        FolderPin folderPin{pin.appId, {ILCloneFull(pin.pidl.get()), PidlFree{}}};
        for (std::wstring& key : FolderKeys(folder)) members.emplace(std::move(key), folderPin);
    }
    AcquireSRWLockExclusive(&g_lock);
    g_running.members.swap(members);
    g_haveMembers = !g_running.members.empty();
    // No pins at all is taken for a list that could not be read: then no app turns loose
    // that is pinned after all.
    if (!pins.empty()) {
        g_running.pinned.swap(pinned);
        g_pinsKnown = true;
    }
    g_looseReady = g_pinsKnown && g_running.looseFolder.shortcut && !g_unloading;
    ReleaseSRWLockExclusive(&g_lock);
    bloom_link::Nudge();
}

std::vector<std::wstring> MemberKeys() {
    std::vector<std::wstring> keys;
    AcquireSRWLockShared(&g_lock);
    for (const auto& [key, folderPin] : g_running.members) keys.push_back(key);
    ReleaseSRWLockShared(&g_lock);
    return keys;
}

struct RESOLVEDWINDOW {  // as taskbar.dll passes it
    HWND hButtonWnd;
    WCHAR szPathStr[MAX_PATH];
    WCHAR szAppIdStr[MAX_PATH];
    ITEMIDLIST* pAppItemIdList;
    HWND hThumbInsertBeforeWnd;
    HWND hThumbParentWnd;
    BOOL bSetPinnableAndLaunchable;
    BOOL bSetThumbFlag;
};

// Set when Start ran in an explorer that had no taskbar yet (explorer starting up): the
// taskbar's first resolved window starts the folders then.
std::atomic<bool> g_startPending;
void BeginStart(bool afterUpdate);

// The passing app ID Reannounce gives a window for a moment.
constexpr wchar_t kPassingAppId[] = L"TaskbarFolders.Regroup";

// Under g_lock. Windows a worker waits on -> the resolves the hook has seen of each since
// it was first watched, under the passing ID and under any other, and which came last.
// While any is watched, the hook wakes the waiters on g_resolved after each resolve.
struct Resolves {
    int watchers = 0, passing = 0, other = 0;
    bool lastPassing = false;
};
std::unordered_map<HWND, Resolves> g_watched;
CONDITION_VARIABLE g_resolved = CONDITION_VARIABLE_INIT;

// Sends a resolved window to its folder's group: the folder's app ID and shortcut in place
// of its own. Every resolve passes here, so the window record follows: a window that stops
// being a member leaves its folder here.
void JoinFolder(RESOLVEDWINDOW* window) {
    if (!g_unloading && g_startPending.exchange(false)) BeginStart(false);
    if (!window) return;
    bool looseReady = g_looseReady;
    // By app ID only: matching the program too would pull in other apps that share it,
    // such as a browser's web apps.
    std::wstring key =
        (g_haveMembers || looseReady) && !g_unloading ? AppKey(window->szAppIdStr) : L"";
    FolderPin folderPin;
    bool joins = false, joined = false, left = false, loose = false;
    AcquireSRWLockExclusive(&g_lock);
    if (auto it = g_watched.find(window->hButtonWnd); it != g_watched.end()) {
        it->second.lastPassing = _wcsicmp(window->szAppIdStr, kPassingAppId) == 0;
        (it->second.lastPassing ? it->second.passing : it->second.other)++;
    }
    bool watched = !g_watched.empty();
    if (auto it = key.empty() ? g_running.members.end() : g_running.members.find(key);
        it != g_running.members.end()) {
        folderPin = it->second;
        joins = true;
    } else if (key.starts_with(Upper(kAppIdPrefix))) {
        // Already sent: the inner resolve after this one sees the folder's ID.
        for (const auto& [member, pin] : g_running.members)
            if (_wcsicmp(pin.appId.c_str(), window->szAppIdStr) == 0) {
                folderPin = pin;
                joined = true;
                break;
            }
        const std::wstring& looseId = g_running.looseFolder.appId;
        if (!joined && !looseId.empty() && _wcsicmp(looseId.c_str(), window->szAppIdStr) == 0)
            joined = true;
    } else if (looseReady && !key.empty() && g_running.looseFolder.shortcut &&
               !g_running.pinned.contains(key)) {
        // Neither pinned nor in a folder: the loose folder takes it and keeps what it was.
        folderPin = g_running.looseFolder;
        joins = loose = true;
        std::erase_if(g_running.loose, [](const auto& entry) { return !IsWindow(entry.first); });
        auto [it, fresh] = g_running.loose.try_emplace(window->hButtonWnd);
        if (fresh) it->second.order = ++g_running.looseOrder;
        it->second.key = key;
        it->second.appId = window->szAppIdStr;
        it->second.path = window->szPathStr;
        it->second.item = {window->pAppItemIdList ? ILCloneFull(window->pAppItemIdList) : nullptr,
                           PidlFree{}};
    }
    if (joins)
        g_running.windows[window->hButtonWnd] = key;
    else if (!joined)
        left = g_running.windows.erase(window->hButtonWnd) > 0;
    if (!loose && !joined) g_running.loose.erase(window->hButtonWnd);
    ReleaseSRWLockExclusive(&g_lock);
    if (watched) WakeAllConditionVariable(&g_resolved);
    if (loose) bloom_link::Nudge();
    if (joins) {
        Wh_Log(L"window of %s joins %s", window->szAppIdStr, folderPin.appId.c_str());
        // The pinned button takes a window only when both its app ID and its shortcut
        // match (an app ID alone made a second button); the taskbar frees the list.
        if (window->pAppItemIdList) ILFree(window->pAppItemIdList);
        window->pAppItemIdList = folderPin.shortcut
                                     ? reinterpret_cast<ITEMIDLIST*>(ILCloneFull(folderPin.shortcut.get()))
                                     : nullptr;
        lstrcpynW(window->szAppIdStr, folderPin.appId.c_str(), MAX_PATH);
    } else if (left && !g_unloading) {
        Wh_Log(L"window of %s leaves its folder", window->szAppIdStr);
    }
}

// The taskbar resolves every window here first; it skips _HandleItemResolved for a window
// whose new ID equals its group's, which kept a window that once got its own button out
// of its folder for good. Answering here, before that comparison, moves it back.
using HandleWindowResolved_t = void(WINAPI*)(void* pThis, RESOLVEDWINDOW* window);
HandleWindowResolved_t HandleWindowResolved_Original;
void WINAPI HandleWindowResolved_Hook(void* pThis, RESOLVEDWINDOW* window) {
    if (window && !IsWindowVisible(window->hButtonWnd))
        Wh_Log(L"hidden window %p resolves as %s", window->hButtonWnd, window->szAppIdStr);
    JoinFolder(window);
    HandleWindowResolved_Original(pThis, window);
}

// The other ways in (packaged apps, snap groups) still pass here.
using HandleItemResolved_t = void(WINAPI*)(void* pThis, RESOLVEDWINDOW* window, void* taskListUI,
                                           void* taskGroup, void* taskItem);
HandleItemResolved_t HandleItemResolved_Original;
void WINAPI HandleItemResolved_Hook(void* pThis, RESOLVEDWINDOW* window, void* taskListUI,
                                    void* taskGroup, void* taskItem) {
    JoinFolder(window);
    HandleItemResolved_Original(pThis, window, taskListUI, taskGroup, taskItem);
}

using HandleTaskGroupSwitchItemAdded_t = void(WINAPI*)(void* pThis, void* switchItem);
HandleTaskGroupSwitchItemAdded_t HandleTaskGroupSwitchItemAdded_Original;
// Snap groups build task items under the windows' own app IDs, which the taskbar no
// longer has for grouped windows, and that crashes explorer. They are skipped while
// any folder holds an open window, unloading or not.
void WINAPI HandleTaskGroupSwitchItemAdded_Hook(void* pThis, void* switchItem) {
    AcquireSRWLockShared(&g_lock);
    bool grouped = std::any_of(g_running.windows.begin(), g_running.windows.end(),
                               [](const auto& entry) { return IsWindow(entry.first); });
    ReleaseSRWLockShared(&g_lock);
    if (grouped) return;
    HandleTaskGroupSwitchItemAdded_Original(pThis, switchItem);
}

// Has the hook count the resolves of `windows` (in g_watched) while it lives.
struct WatchResolves {
    std::vector<HWND> windows;
    explicit WatchResolves(const std::vector<HWND>& watched) : windows(watched) {
        AcquireSRWLockExclusive(&g_lock);
        for (HWND window : windows) g_watched[window].watchers++;
        ReleaseSRWLockExclusive(&g_lock);
    }
    ~WatchResolves() {
        AcquireSRWLockExclusive(&g_lock);
        for (HWND window : windows)
            if (auto it = g_watched.find(window); it != g_watched.end() && --it->second.watchers <= 0)
                g_watched.erase(it);
        ReleaseSRWLockExclusive(&g_lock);
    }
    WatchResolves(const WatchResolves&) = delete;
    WatchResolves& operator=(const WatchResolves&) = delete;
};

Resolves SeenResolves(HWND window) {
    AcquireSRWLockShared(&g_lock);
    auto it = g_watched.find(window);
    Resolves seen = it != g_watched.end() ? it->second : Resolves{};
    ReleaseSRWLockShared(&g_lock);
    return seen;
}

// Off the taskbar thread, while a window is watched: waits up to `ms` until `done` (called under
// the shared g_lock) holds, looking again after each resolve the hook sees.
template <typename Done>
bool AwaitHook(DWORD ms, Done done) {
    ULONGLONG end = GetTickCount64() + ms;
    AcquireSRWLockShared(&g_lock);
    bool held = done();
    for (ULONGLONG now = GetTickCount64(); !held && now < end; now = GetTickCount64()) {
        SleepConditionVariableSRW(&g_resolved, &g_lock, DWORD(end - now),
                                  CONDITION_VARIABLE_LOCKMODE_SHARED);
        held = done();
    }
    ReleaseSRWLockShared(&g_lock);
    return held;
}

// Makes the taskbar resolve `windows` again, so they move to or from a folder's group.
// Two ways, as each reaches windows the other misses (seen from outside explorer):
// taking the button out and back in through ITaskbarList re-resolves most apps, and a
// change of the window's app ID property (to a passing value and straight back)
// re-resolves packaged ones. Replayed shell hook messages did nothing.
// From inside explorer the steps arrive faster than from outside and the taskbar dropped
// most of them, so each waits for the taskbar, at most kStepMs: after the button is back
// and after the passing ID, until the hook sees the window resolve under it. A window the
// hook saw resolve after its button came back is in its group, and keeps its ID.
// shortcut: no hook tells when a button is gone, so that step keeps its fixed wait.
// `together`: every button goes before any comes back, so a group they share is made
// anew, at the end; one by one, the group keeps the place it has.
void Reannounce(const std::vector<HWND>& windows, bool together) {
    if (windows.empty()) return;
    ULONGLONG start = GetTickCount64();
    ComPtr<ITaskbarList> tabs;
    if (FAILED(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(tabs.GetAddressOf()))) ||
        FAILED(tabs->HrInit()))
        tabs = nullptr;
    constexpr DWORD kStepMs = 120;
    WatchResolves watch(windows);
    int readded = 0, retagged = 0, kept = 0, noticed = 0, timedOut = 0;
    std::vector<HWND> out;  // with `together`: the buttons taken out already
    if (tabs && together) {
        for (HWND window : windows)
            if (SUCCEEDED(tabs->DeleteTab(window))) out.push_back(window);
        if (!out.empty()) Sleep(kStepMs);
    }
    // Waits until the hook has seen `window` resolve once more than `before`, under the
    // passing ID or under another.
    auto await = [&](HWND window, const Resolves& before, bool passing) {
        bool seen = AwaitHook(kStepMs, [&] {
            auto it = g_watched.find(window);
            return it != g_watched.end() && (passing ? it->second.passing > before.passing
                                                     : it->second.other > before.other);
        });
        (seen ? noticed : timedOut)++;
        return seen;
    };
    for (HWND window : windows) {
        bool gone = together ? std::find(out.begin(), out.end(), window) != out.end()
                             : tabs && SUCCEEDED(tabs->DeleteTab(window));
        if (gone) {
            if (!together) Sleep(kStepMs);
            Resolves before = SeenResolves(window);
            if (SUCCEEDED(tabs->AddTab(window))) {
                readded++;
                if (await(window, before, false)) {
                    kept++;
                    continue;
                }
            } else {
                Sleep(kStepMs);
            }
        }
        ComPtr<IPropertyStore> store;
        PROPVARIANT own{};
        if (FAILED(SHGetPropertyStoreForWindow(window, IID_PPV_ARGS(store.GetAddressOf()))) ||
            FAILED(store->GetValue(kAppUserModelId, &own)))
            continue;
        PROPVARIANT passing{};
        passing.vt = VT_LPWSTR;
        passing.pwszVal = const_cast<PWSTR>(kPassingAppId);
        Resolves before = SeenResolves(window);
        if (SUCCEEDED(store->SetValue(kAppUserModelId, passing))) {
            await(window, before, true);
            if (SUCCEEDED(store->SetValue(kAppUserModelId, own)))  // VT_EMPTY clears it again
                retagged++;
        }
        PropVariantClear(&own);
    }
    Wh_Log(L"%d windows: %d buttons re-added, %d IDs retagged, %d kept theirs; %d waits ended "
           L"by the taskbar, %d timed out; %d ms",
           int(windows.size()), readded, retagged, kept, noticed, timedOut,
           int(GetTickCount64() - start));
}

// Top-level windows with a taskbar button, topmost first.
std::vector<HWND> TaskbarWindows() {
    std::vector<HWND> windows;
    EnumWindows(
        [](HWND window, LPARAM param) {
            LONG_PTR exStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
            if (IsWindowVisible(window) && !(exStyle & WS_EX_TOOLWINDOW) &&
                (!GetWindow(window, GW_OWNER) || (exStyle & WS_EX_APPWINDOW)))
                reinterpret_cast<std::vector<HWND>*>(param)->push_back(window);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));
    return windows;
}

// Windows an earlier copy of the mod let go of (see ReleaseGroupedWindows), shown or hidden.
std::vector<HWND> ReleasedWindows() {
    std::vector<HWND> windows;
    EnumWindows(
        [](HWND window, LPARAM param) {
            if (GetPropW(window, kReleasedProp))
                reinterpret_cast<std::vector<HWND>*>(param)->push_back(window);
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));
    return windows;
}

// The shell's app resolver: the app ID the taskbar gives a window, Store apps and apps
// that set their ID for the whole process included.
constexpr GUID kClsidAppResolver = {
    0x660b90c8, 0x73a9, 0x4b58, {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};
constexpr GUID kIidAppResolver8 = {
    0xde25675a, 0x72de, 0x44b4, {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

struct IAppResolver8 : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut(IShellItem* item, PWSTR* appId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForWindow(HWND window, PWSTR* appId, void*, void*,
                                                        void*) = 0;
};

ComPtr<IAppResolver8> AppResolver() {
    ComPtr<IAppResolver8> resolver;
    CoCreateInstance(kClsidAppResolver, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                     kIidAppResolver8, reinterpret_cast<void**>(resolver.GetAddressOf()));
    return resolver;
}

// The window's own app key, as the taskbar would resolve it without the mod.
std::wstring WindowAppKey(IAppResolver8* resolver, HWND window) {
    PWSTR appId = nullptr;
    if (!resolver || FAILED(resolver->GetAppIDForWindow(window, &appId, nullptr, nullptr, nullptr)))
        return {};
    std::wstring key = AppKey(appId);
    CoTaskMemFree(appId);
    return key;
}

// After folder contents change: the member table follows, and open windows of the
// apps with `keys` move to their new group. The hook updates the window records as
// the taskbar takes the windows again.
void Regroup(const std::vector<std::wstring>& keys) {
    RebuildMembers();
    if (keys.empty()) return;
    auto wanted = [&](const std::wstring& key) {
        return !key.empty() && std::find(keys.begin(), keys.end(), key) != keys.end();
    };
    std::vector<HWND> windows;
    ComPtr<IAppResolver8> resolver = AppResolver();
    for (HWND window : TaskbarWindows())
        if (wanted(WindowAppKey(resolver.Get(), window))) windows.push_back(window);
    for (HWND window : ReleasedWindows())
        if (wanted(WindowAppKey(resolver.Get(), window))) {
            RemovePropW(window, kReleasedProp);
            if (std::find(windows.begin(), windows.end(), window) == windows.end())
                windows.push_back(window);
        }
    AcquireSRWLockShared(&g_lock);
    for (const auto& [window, key] : g_running.windows)
        if (wanted(key) && IsWindow(window) &&
            std::find(windows.begin(), windows.end(), window) == windows.end())
            windows.push_back(window);
    ReleaseSRWLockShared(&g_lock);
    std::vector<std::wstring> windowKeys;  // resolved outside the lock the hook waits on
    for (HWND window : windows) windowKeys.push_back(WindowAppKey(resolver.Get(), window));
    WatchResolves watch(windows);
    Reannounce(windows, false);
    // Members' windows the taskbar has not handed to the hook within half a second get
    // one more go. The wait ends early once each has resolved under its own ID last and
    // is in a folder: a record from before the move does not count.
    std::vector<HWND> missed;
    AwaitHook(500, [&] {
        bool settled = true;
        missed.clear();
        for (size_t i = 0; i < windows.size(); i++) {
            if (!g_running.members.contains(windowKeys[i]) || !IsWindow(windows[i])) continue;
            auto seen = g_watched.find(windows[i]);
            if (!g_running.windows.contains(windows[i]))
                missed.push_back(windows[i]);
            else if (seen != g_watched.end() && seen->second.other && !seen->second.lastPassing)
                continue;
            settled = false;
        }
        return settled;
    });
    if (!missed.empty() && !g_unloading) Reannounce(missed, false);
}

// Open windows of a folder item, topmost first.
std::vector<HWND> ItemWindows(const std::wstring& dir, const std::wstring& fileName) {
    std::vector<std::wstring> keys = ItemKeys(dir, fileName);
    std::vector<HWND> grouped;
    AcquireSRWLockShared(&g_lock);
    for (const auto& [window, key] : g_running.windows)
        if (std::find(keys.begin(), keys.end(), key) != keys.end()) grouped.push_back(window);
    ReleaseSRWLockShared(&g_lock);
    std::vector<HWND> windows;
    for (HWND window : TaskbarWindows())
        if (std::find(grouped.begin(), grouped.end(), window) != grouped.end())
            windows.push_back(window);
    return windows;
}

// Brings `window` (or the dialog it is showing) to the front.
void SwitchToWindow(HWND window) {
    HWND active = window;
    HWND popup = GetLastActivePopup(GetAncestor(window, GA_ROOTOWNER));
    if (popup && popup != window && IsWindowVisible(popup) && IsWindowEnabled(popup))
        active = popup;
    if (IsIconic(window)) ShowWindowAsync(window, SW_RESTORE);
    SwitchToThisWindow(active, TRUE);
}

// One of an app's open windows (topmost first) comes to the front. By z-order, as the
// click itself took the foreground: when the app's top window is the frontmost app
// window, its bottom one comes next, which cycles through them.
void CycleWindows(const std::vector<HWND>& windows) {
    HWND front = nullptr;
    for (HWND window : TaskbarWindows())
        if (!(GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
            front = window;
            break;
        }
    SwitchToWindow(front == windows.front() ? windows.back() : windows.front());
}

// A running app comes to the front, the next of its windows on each click; any
// other item starts.
void OpenOrSwitch(HWND owner, const std::wstring& path) {
    std::wstring dir = path.substr(0, path.rfind(L'\\'));
    std::vector<HWND> windows = ItemWindows(dir, PathFindFileNameW(path.c_str()));
    if (windows.empty()) return Open(owner, path);
    CycleWindows(windows);
}

// The app ID a folder item's jump list goes by: the taskbar's from when it was pinned,
// else what the shell resolves the shortcut to; the resolver knows no Apps folder items.
std::wstring ItemAppId(const std::wstring& dir, const std::wstring& fileName) {
    std::wstring appId = RecordedAppId(dir, fileName);
    ComPtr<IAppResolver8> resolver = appId.empty() ? AppResolver() : nullptr;
    ComPtr<IShellItem> item;
    PWSTR resolved = nullptr;
    if (resolver &&
        SUCCEEDED(SHCreateItemFromParsingName((dir + L"\\" + fileName).c_str(), nullptr,
                                              IID_PPV_ARGS(item.GetAddressOf()))) &&
        SUCCEEDED(resolver->GetAppIDForShortcut(item.Get(), &resolved)) && resolved) {
        appId = resolved;
        CoTaskMemFree(resolved);
    }
    if (ComPtr<IShellLinkW> link = appId.empty() ? LoadShellLink(dir + L"\\" + fileName) : nullptr)
        appId = AppsFolderId(link.Get());
    return appId;
}

// A recent or frequent document of an app, as its jump list shows it.
struct Document {
    std::wstring label, arguments;
    std::shared_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>> item;
    Pixels icon;
    int iconSize = 0;
};

// Up to `max` of the app's documents, recent ones first. Tasks and pinned entries an app
// puts in its jump list have no public reader, so they are not here.
std::vector<Document> AppDocuments(const std::wstring& appId, int iconSize, size_t max) {
    std::vector<Document> documents;
    ComPtr<IApplicationDocumentLists> lists;
    if (appId.empty() ||
        FAILED(CoCreateInstance(CLSID_ApplicationDocumentLists, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(lists.GetAddressOf()))) ||
        FAILED(lists->SetAppID(appId.c_str())))
        return documents;
    for (APPDOCLISTTYPE type : {ADLT_RECENT, ADLT_FREQUENT}) {
        ComPtr<IObjectArray> array;
        UINT count = 0;
        if (FAILED(lists->GetList(type, UINT(max), IID_PPV_ARGS(array.GetAddressOf()))) ||
            FAILED(array->GetCount(&count)))
            continue;
        for (UINT i = 0; i < count && documents.size() < max; i++) {
            Document document;
            PIDLIST_ABSOLUTE pidl = nullptr;
            ComPtr<IShellItem> item;
            ComPtr<IShellLinkW> link;
            if (SUCCEEDED(array->GetAt(i, IID_PPV_ARGS(item.GetAddressOf())))) {
                SHGetIDListFromObject(item.Get(), &pidl);
            } else if (SUCCEEDED(array->GetAt(i, IID_PPV_ARGS(link.GetAddressOf())))) {
                link->GetIDList(&pidl);
                wchar_t arguments[INFOTIPSIZE] = L"";
                if (SUCCEEDED(link->GetArguments(arguments, ARRAYSIZE(arguments))))
                    document.arguments = arguments;
                if (pidl) SHCreateItemFromIDList(pidl, IID_PPV_ARGS(item.GetAddressOf()));
            }
            if (!pidl) continue;
            document.item = {pidl, PidlFree{}};
            if (std::any_of(documents.begin(), documents.end(), [&](const Document& seen) {
                    return ILIsEqual(seen.item.get(), pidl) && seen.arguments == document.arguments;
                }))
                continue;
            PWSTR label = nullptr;
            if (FAILED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &label))) continue;
            document.label = label;
            CoTaskMemFree(label);
            if (IconPixels(item.Get(), iconSize, &document.icon)) document.iconSize = iconSize;
            documents.push_back(std::move(document));
        }
    }
    return documents;
}

void OpenDocument(const Document& document) {
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC;  // the calling thread exits right after
    sei.lpIDList = document.item.get();
    sei.lpParameters = document.arguments.empty() ? nullptr : document.arguments.c_str();
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

// As the taskbar's "close window": each window is asked, so it can save first.
void CloseWindows(const std::vector<HWND>& windows) {
    for (HWND window : windows) PostMessageW(window, WM_SYSCOMMAND, SC_CLOSE, 0);
}

// Gives grouped windows their own buttons back, while the hook still runs: it skips
// snap groups until each window is taken again. Each keeps a mark, so the next copy of the
// mod takes back these windows, hidden ones with a button included, which it could not
// tell apart from hidden windows without one. The taskbar's window notes when, for the
// next copy's start.
void ReleaseGroupedWindows() {
    std::vector<HWND> windows;
    AcquireSRWLockExclusive(&g_lock);
    for (const auto& [window, key] : g_running.windows)
        if (IsWindow(window)) windows.push_back(window);
    g_running.members.clear();
    g_haveMembers = false;
    g_looseReady = false;
    ReleaseSRWLockExclusive(&g_lock);
    for (HWND window : windows) SetPropW(window, kReleasedProp, HANDLE(1));
    Reannounce(windows, false);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD process = 0;
    if (!windows.empty() && tray && GetWindowThreadProcessId(tray, &process) &&
        process == GetCurrentProcessId())
        SetPropW(tray, kReleasedAtProp, HANDLE(ULONG_PTR(GetTickCount64())));
}

// ---- Loose folder (worker threads with COM) ---------------------------------

// Open windows in the loose folder, of the app with `key` or of every app, topmost first.
std::vector<HWND> LooseWindows(const std::wstring& key) {
    std::vector<HWND> grouped;
    AcquireSRWLockShared(&g_lock);
    for (const auto& [window, loose] : g_running.loose)
        if (key.empty() || loose.key == key) grouped.push_back(window);
    ReleaseSRWLockShared(&g_lock);
    std::vector<HWND> windows;
    for (HWND window : TaskbarWindows())
        if (std::find(grouped.begin(), grouped.end(), window) != grouped.end())
            windows.push_back(window);
    return windows;
}

void SwitchToLoose(const std::wstring& key) {
    std::vector<HWND> windows = LooseWindows(key);
    if (!windows.empty()) CycleWindows(windows);
}

// A loose app's shell item: the app item the taskbar resolved, else its program, else
// its entry in the Apps folder.
ComPtr<IShellItem> LooseShellItem(const LooseWindow& app) {
    ComPtr<IShellItem> item;
    if (app.item) SHCreateItemFromIDList(app.item.get(), IID_PPV_ARGS(item.GetAddressOf()));
    if (!item && !app.path.empty())
        SHCreateItemFromParsingName(app.path.c_str(), nullptr, IID_PPV_ARGS(item.GetAddressOf()));
    if (!item && !app.appId.empty())
        SHCreateItemFromParsingName((L"shell:AppsFolder\\" + app.appId).c_str(), nullptr,
                                    IID_PPV_ARGS(item.GetAddressOf()));
    return item;
}

// PKEY_FileDescription
constexpr PROPERTYKEY kFileDescription = {
    {0x0CEF7D53, 0xFA64, 0x11D1, {0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE}}, 3};

// An app's name: a program by its own description ("Notepad", not "notepad.exe"), any
// other item by its display name.
std::wstring AppLabel(IShellItem* item) {
    std::wstring label;
    PWSTR path = nullptr, text = nullptr;
    ComPtr<IShellItem2> details;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) &&
        PathMatchSpecW(path, L"*.exe") &&
        SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(details.GetAddressOf()))) &&
        SUCCEEDED(details->GetString(kFileDescription, &text)) && text) {
        label = text;
        CoTaskMemFree(text);
        text = nullptr;
    }
    CoTaskMemFree(path);
    if (label.empty() && SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &text))) {
        label = text;
        CoTaskMemFree(text);
    }
    return label;
}

// Starts another copy of a loose app, as its own button would.
void OpenLooseApp(const LooseWindow& app) {
    if (app.item) return OpenDocument({L"", L"", app.item});
    Open(nullptr, !app.path.empty() ? app.path : L"shell:AppsFolder\\" + app.appId);
}

// Pins a loose app at the end of the taskbar, or after the last pinned app of `leftOf` (a
// drop between buttons); its windows then leave the folder for the new pin's button.
void PinLoose(const LooseWindow& app,
              const std::optional<std::vector<std::wstring>>& leftOf = std::nullopt) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    Pidl program = app.item ? nullptr : PidlFromPath(app.path);
    PCIDLIST_ABSOLUTE item = app.item ? app.item.get() : program.get();
    if (!manager || !item || g_unloading) return;
    std::vector<Pin> pins = ReadPins();
    if (PinAt(manager.Get(), item, leftOf ? PinIndexAfter(pins, *leftOf) : int(pins.size())))
        Regroup({app.key});
}

// Windows with a taskbar button as far as their styles tell: none cloaked (suspended Store
// apps, other desktops), untitled or the desktop. Only these may be sent through the
// taskbar again, which gives any window it is handed a button.
std::vector<HWND> ButtonWindows() {
    std::vector<HWND> windows;
    HWND desktop = GetShellWindow();
    for (HWND window : TaskbarWindows()) {
        DWORD cloaked = 0;
        if (window == desktop || !GetWindowTextLengthW(window) ||
            (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
             cloaked))
            continue;
        windows.push_back(window);
    }
    return windows;
}

// After the loose folder changed (start, its setting or name): windows go into it or out
// of it where they belong elsewhere now, and with `all` every window in it goes again,
// all out before any is back, so its button is made anew at the end.
void RegroupLoose(bool all) {
    ComPtr<IAppResolver8> resolver = AppResolver();
    std::vector<HWND> windows = ButtonWindows(), released = ReleasedWindows();
    auto add = [&](HWND window) {
        if (std::find(windows.begin(), windows.end(), window) == windows.end())
            windows.push_back(window);
    };
    for (HWND window : released) add(window);
    AcquireSRWLockShared(&g_lock);
    std::vector<HWND> inside;  // hidden ones too
    for (const auto& [window, loose] : g_running.loose)
        if (IsWindow(window)) inside.push_back(window);
    ReleaseSRWLockShared(&g_lock);
    for (HWND window : inside) add(window);
    std::vector<std::wstring> keys;  // resolved outside the lock the hook waits on
    for (HWND window : windows) keys.push_back(WindowAppKey(resolver.Get(), window));
    std::wstring prefix = Upper(kAppIdPrefix);
    std::vector<HWND> moving;
    AcquireSRWLockShared(&g_lock);
    for (size_t i = 0; i < windows.size(); i++) {
        bool in = g_running.loose.contains(windows[i]);
        bool belongs = g_looseReady && !keys[i].empty() && !keys[i].starts_with(prefix) &&
                       !g_running.members.contains(keys[i]) && !g_running.pinned.contains(keys[i]);
        if (belongs != in || (in && all)) moving.push_back(windows[i]);
    }
    ReleaseSRWLockShared(&g_lock);
    for (HWND window : moving)
        if (std::find(released.begin(), released.end(), window) != released.end())
            RemovePropW(window, kReleasedProp);
    if (!moving.empty()) Wh_Log(L"%d windows move for the loose folder", int(moving.size()));
    Reannounce(moving, all);
}

// The taskbar's theme picks the tile, as the folder icon sets have a light and a dark one:
// the silver tile stands out on a dark taskbar, the graphite one on a light taskbar.
bool LightTaskbar() {
    return !settings::Dark();
}

// An icon in two looks is a pair of files, `<name>.dark.ico` for a dark taskbar beside
// `<name>.light.ico` for a light one. Of the pair `path` belongs to, the file `light`
// asks for; empty when `path` is no such pair or that file is gone.
std::wstring ThemedTwin(const std::wstring& path, bool light) {
    auto endsWith = [&](std::wstring_view end) {
        return path.size() > end.size() &&
               _wcsicmp(path.c_str() + path.size() - end.size(), end.data()) == 0;
    };
    constexpr std::wstring_view kDark = L".dark.ico", kLight = L".light.ico";
    size_t cut = endsWith(kDark) ? kDark.size() : endsWith(kLight) ? kLight.size() : 0;
    if (!cut) return {};
    std::wstring twin = path.substr(0, path.size() - cut) + std::wstring(light ? kLight : kDark);
    return GetFileAttributesW(twin.c_str()) != INVALID_FILE_ATTRIBUTES ? twin : L"";
}

// Pinned folders whose icon is such a pair take the look of the taskbar's theme. True when
// one was pinned again; the loose folder is left where that put it.
bool RethemeIcons() {
    bool light = LightTaskbar(), repinned = false;
    for (const Pin& pin : ReadPins()) {
        std::wstring folder = FolderOfPin(pin);
        if (folder.empty() || g_unloading) continue;
        IconLocation now = PinIcon(folder);
        std::wstring want = ThemedTwin(now.path, light);
        if (!want.empty() && _wcsicmp(want.c_str(), now.path.c_str()) != 0 &&
            SetFolderIcon(folder, {want}, false))
            repinned = true;
    }
    return repinned;
}

// Deletes the files in `dir` (the loose folder's own) and in its folders except `keep`,
// and each folder left empty.
void CleanLooseDir(const std::wstring& dir, const std::vector<std::wstring>& keep) {
    if (dir.empty()) return;
    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            std::wstring path = dir + L"\\" + fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (wcscmp(fd.cFileName, L".") != 0 && wcscmp(fd.cFileName, L"..") != 0 &&
                    !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
                    CleanLooseDir(path, keep);
            } else if (std::none_of(keep.begin(), keep.end(), [&](const std::wstring& kept) {
                           return _wcsicmp(kept.c_str(), path.c_str()) == 0;
                       })) {
                DeleteFileW(path.c_str());
            }
        } while (FindNextFileW(find, &fd));
        FindClose(find);
    }
    RemoveDirectoryW(dir.c_str());  // fails while something kept is in it
}

SRWLOCK g_looseWork = SRWLOCK_INIT;  // one worker at a time sets up the loose folder
std::wstring g_looseDir;             // under g_looseWork: where its files went last

// The loose folder's shortcut and clock icon, as the settings ask. Its app ID changes with
// its name and tile, so a change makes a new button rather than leaving the old look.
void PrepareLoose() {
    AcquireSRWLockShared(&g_lock);
    bool on = g_looseOn;
    std::wstring name = g_looseName;
    ReleaseSRWLockShared(&g_lock);
    std::wstring dir = Root().empty() ? L"" : PinsDir() + L"\\" + kLooseDir;
    FolderPin folder;
    std::vector<std::wstring> keep;
    if (on && !dir.empty() && !g_unloading) {
        bool light = LightTaskbar();
        std::wstring look = light ? L"light" : L"dark";
        // "-2": the drawing's version; a new drawing needs a new name past the icon cache.
        std::wstring icon = dir + L"\\clock-" + look + L"-2.ico";
        // The taskbar keeps the icon it read for a shortcut's path, so each tile's shortcut
        // has a folder of its own, under the same name.
        std::wstring lnk = dir + L"\\" + look + L"\\" + name + L".lnk";
        std::wstring appId = kLooseAppId + HashHex(name + L"\n" + look);
        CreateDirectoryW(dir.c_str(), nullptr);
        CreateDirectoryW((dir + L"\\" + look).c_str(), nullptr);
        bool ok = (GetFileAttributesW(icon.c_str()) != INVALID_FILE_ATTRIBUTES ||
                   WriteFileBytes(icon, EncodeIco(ClockIcon(!light), kIconCanvas))) &&
                  WriteExplorerLnk(lnk, L"", {icon}, appId);
        if (ok) NotifyUpdated(lnk);  // a rewritten file, told at once
        if (Pidl shortcut = ok ? PidlFromPath(lnk) : nullptr) {
            folder = {appId, {shortcut.release(), PidlFree{}}};
            keep = {icon, lnk};
        }
        Wh_Log(L"loose folder %s: %d", appId.c_str(), ok);
    }
    AcquireSRWLockExclusive(&g_lock);
    g_running.looseFolder = folder;
    g_looseReady = g_pinsKnown && folder.shortcut && !g_unloading;
    ReleaseSRWLockExclusive(&g_lock);
    CleanLooseDir(dir, keep);
    if (_wcsicmp(g_looseDir.c_str(), dir.c_str()) != 0) CleanLooseDir(g_looseDir, {});
    g_looseDir = dir;
}

std::wstring LooseAppId() {
    AcquireSRWLockShared(&g_lock);
    std::wstring appId = g_running.looseFolder.appId;
    ReleaseSRWLockShared(&g_lock);
    return appId;
}

// After its settings may have changed: the loose folder's files, then its windows, which
// all go again when it changed or `last` asks for it at the end.
void UpdateLoose(bool last) {
    AcquireSRWLockExclusive(&g_looseWork);
    std::wstring before = LooseAppId();
    PrepareLoose();
    RegroupLoose(last || LooseAppId() != before);
    ReleaseSRWLockExclusive(&g_looseWork);
}

// ---- Folder operations (worker threads with COM) --------------------------

// Only shortcuts go back to the taskbar: other files in a folder are the user's own,
// and taking one out would send the original to the Recycle Bin.
bool IsShortcut(const std::wstring& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY) &&
           PathMatchSpecW(path.c_str(), L"*.lnk");
}

// Drop of a pinned app onto another pinned app or folder. Returns the app ID of an
// existing folder that took the app, for its "got it" bounce.
std::wstring MergeApps(const std::wstring& draggedId, const std::wstring& targetId) {
    Sleep(kPinSettleMs);
    if (g_unloading) return {};
    std::wstring root = Root();
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int dragged = FindPin(pins, draggedId), target = FindPin(pins, targetId);
    Wh_Log(L"merge %s (%d) onto %s (%d)", draggedId.c_str(), dragged, targetId.c_str(), target);
    if (root.empty() || !manager || dragged < 0 || target < 0 || dragged == target) return {};

    std::wstring folder = FolderOfPin(pins[target]);
    bool newFolder = folder.empty();
    if (newFolder) {
        std::vector<std::wstring> taken;
        for (const Entry& entry : ListEntries(root)) taken.push_back(entry.name);
        folder = root + L"\\" + NextFolderName(taken);
        if (!CreateDirectoryW(folder.c_str(), nullptr)) return {};
        if (!CopyPinInto(pins[target], folder)) {
            RemoveDirectoryW(folder.c_str());
            return {};
        }
    }
    if (!CopyPinInto(pins[dragged], folder)) {
        if (newFolder) RemoveFolder(folder);
        return {};
    }

    if (newFolder) {
        bool written = WritePinLnk(folder, {MakePreviewIcon(folder)});
        if (written) ExpectFolderButton(FolderAppId(folder), targetId);
        if (!written ||
            !ReplacePin(manager.Get(), pins, target, PidlFromPath(PinLnkPath(folder)).get())) {
            g_mergeMotionPending = false;
            RemoveFolder(folder);  // nothing on the taskbar changed yet
            return {};
        }
    }
    Unpin(manager.Get(), pins[dragged].pidl.get());
    Regroup(FolderKeys(folder));
    if (newFolder) return {};
    RefreshPreview(folder);
    return FolderAppId(folder);
}

// After an app left `folder`: with one app left, that app takes the folder's place and the
// folder goes; otherwise the folder's icon follows its apps. Returns the keys of the apps
// whose windows leave the folder.
std::vector<std::wstring> SettleSource(IPinManagerInterop3* manager, const std::wstring& folder) {
    std::vector<MenuItem> rest = LoadItems(folder, 0);
    if (manager && rest.size() == 1 && IsShortcut(rest[0].path)) {
        std::vector<std::wstring> last = ItemKeys(folder, rest[0].fileName);
        std::vector<Pin> pins = ReadPins();
        int index = FindFolderPin(pins, folder);
        if (index < 0 || !ReplacePin(manager, pins, index, PinnablePidl(rest[0].path).get())) return {};
        RemoveFolder(folder);
        return last;
    }
    RefreshPreview(folder);
    return {};
}

// Drop of a folder item outside its menu: pin it next to the folder, or after the last
// pinned app of `leftOf` (a drop between buttons).
void TakeOut(const std::wstring& folder, const std::wstring& item,
             const std::optional<std::vector<std::wstring>>& leftOf = std::nullopt) {
    if (!IsShortcut(item)) return;
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (!manager || index < 0 ||
        !PinAt(manager.Get(), PinnablePidl(item).get(),
               leftOf ? PinIndexAfter(pins, *leftOf) : index + 1))
        return;
    std::vector<std::wstring> released = ItemKeys(folder, PathFindFileNameW(item.c_str()));
    Recycle(item);
    std::vector<std::wstring> last = SettleSource(manager.Get(), folder);
    released.insert(released.end(), last.begin(), last.end());
    Regroup(released);
}

// `fileName`, or "<name> (2)<extension>", "(3)", ... while `dir` has that already; empty
// when every number is taken.
std::wstring FreeName(const std::wstring& dir, const std::wstring& fileName) {
    std::wstring extension = PathFindExtensionW(fileName.c_str());
    std::wstring stem = fileName.substr(0, fileName.size() - extension.size());
    std::wstring name = fileName;
    for (int n = 2; GetFileAttributesW((dir + L"\\" + name).c_str()) != INVALID_FILE_ATTRIBUTES; n++) {
        if (n > 99) return {};
        name = stem + L" (" + std::to_wstring(n) + L")" + extension;
    }
    return name;
}

// True when the folder has an item whose app ID is `appId`.
bool HoldsApp(const std::wstring& folder, const std::wstring& appId) {
    if (appId.empty()) return false;
    std::wstring key = AppKey(appId);
    for (const MenuItem& item : LoadItems(folder, 0))
        if (std::wstring id = ItemAppId(folder, item.fileName); !id.empty() && AppKey(id) == key)
            return true;
    return false;
}

// What a shortcut starts: its target (or Apps folder item), arguments and working
// folder, lower case; empty when the file isn't a shortcut that can be read.
std::wstring LaunchOf(const std::wstring& path) {
    ComPtr<IShellLinkW> link = LoadShellLink(path);
    if (!link) return {};
    wchar_t target[MAX_PATH] = L"", args[MAX_PATH * 2] = L"", dir[MAX_PATH] = L"";
    if (link->GetPath(target, MAX_PATH, nullptr, SLGP_RAWPATH) != S_OK) target[0] = L'\0';
    std::wstring what = target[0] ? std::wstring(target) : AppsFolderId(link.Get());
    if (what.empty()) return {};
    link->GetArguments(args, ARRAYSIZE(args));
    link->GetWorkingDirectory(dir, MAX_PATH);
    std::wstring launch = what + L'\n' + args + L'\n' + dir;
    CharLowerBuffW(launch.data(), (DWORD)launch.size());
    return launch;
}

// True when the folder has an item of the same app that starts the same thing as the
// item at `path`: the same program with other arguments (a project, a profile) is
// another item.
bool HoldsSame(const std::wstring& folder, const std::wstring& path, const std::wstring& appId) {
    std::wstring launch = LaunchOf(path);
    if (appId.empty() || launch.empty()) return false;
    std::wstring key = AppKey(appId);
    for (const MenuItem& item : LoadItems(folder, 0))
        if (std::wstring id = ItemAppId(folder, item.fileName);
            !id.empty() && AppKey(id) == key && LaunchOf(folder + L"\\" + item.fileName) == launch)
            return true;
    return false;
}

// A folder item dropped on another folder's button goes into that folder, last in its
// order. The file is moved, never copied and deleted, so it is in one of the two at every
// moment; a name taken there gets a number. A shortcut the folder has already (the same
// app starting the same thing) leaves its source for the Recycle Bin instead. `keys`
// gets the apps whose windows change buttons (Regroup).
bool MoveInto(const std::wstring& folder, const std::wstring& item, const std::wstring& target,
              std::vector<std::wstring>* keys) {
    std::wstring fileName = PathFindFileNameW(item.c_str());
    if (g_unloading || target.empty() || _wcsicmp(folder.c_str(), target.c_str()) == 0 ||
        !PathIsDirectoryW(target.c_str()))
        return false;
    if (HoldsSame(target, item, ItemAppId(folder, fileName))) {
        *keys = ItemKeys(folder, fileName);
        Recycle(item);
        Wh_Log(L"move %s into %s: there already", item.c_str(), target.c_str());
        if (GetFileAttributesW(item.c_str()) != INVALID_FILE_ATTRIBUTES) return false;
        ForgetAppId(folder, fileName);
        WriteOrder(folder, FileNames(LoadItems(folder, 0)));  // without the name that left
        std::vector<std::wstring> last = SettleSource(PinManager().Get(), folder);
        keys->insert(keys->end(), last.begin(), last.end());
        return true;
    }
    std::wstring name = FreeName(target, fileName);
    std::wstring appId = RecordedAppId(folder, fileName);
    bool moved = !name.empty() &&
                 MoveFileExW(item.c_str(), (target + L"\\" + name).c_str(), MOVEFILE_COPY_ALLOWED);
    Wh_Log(L"move %s into %s as %s: %d", item.c_str(), target.c_str(), name.c_str(), moved);
    if (!moved) return false;
    if (!appId.empty()) {
        RecordAppId(target, name, appId);
        ForgetAppId(folder, fileName);
    } else {
        ForgetAppId(target, name);  // a line left by an earlier item of that name
    }
    PutLast(target, name);
    WriteOrder(folder, FileNames(LoadItems(folder, 0)));  // without the name that left
    RefreshPreview(target);
    *keys = ItemKeys(target, name);
    std::vector<std::wstring> last = SettleSource(PinManager().Get(), folder);
    keys->insert(keys->end(), last.begin(), last.end());
    return true;
}

// A loose app dropped on a folder's button joins that folder as a new shortcut to it, or
// only its windows do when the folder has the app already. False when it has no item a
// shortcut can point at.
bool MoveLooseInto(const LooseWindow& app, const std::wstring& target, std::vector<std::wstring>* keys) {
    if (!g_unloading && !target.empty() && HoldsApp(target, app.appId)) {
        *keys = {app.key};
        return true;
    }
    ComPtr<IShellItem> shell = g_unloading || target.empty() ? nullptr : LooseShellItem(app);
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (!shell || FAILED(SHGetIDListFromObject(shell.Get(), &pidl)) || !pidl) return false;
    Pidl item(pidl);
    std::wstring name = AddShortcutInto(item.get(), target, app.appId, true);
    if (name.empty()) return false;
    RefreshPreview(target);
    *keys = ItemKeys(target, name);
    keys->push_back(app.key);
    return true;
}

// Files dropped from outside onto a folder's button: each goes in as a shortcut. True if
// any did. A folder's own pin shortcut stays out: folders hold apps, not folders.
bool DropInto(const std::wstring& folder, const std::vector<std::wstring>& paths) {
    std::vector<std::wstring> keys;
    bool added = false;
    for (const std::wstring& path : paths)
        if (Pidl item = FolderFromLnk(path).empty() ? PidlFromPath(path) : nullptr)
            if (std::wstring name = AddShortcutInto(item.get(), folder); !name.empty()) {
                added = true;
                for (std::wstring& key : ItemKeys(folder, name)) keys.push_back(std::move(key));
            }
    if (!added) return false;
    RefreshPreview(folder);
    Regroup(keys);
    return true;
}

void ReorderItem(const std::wstring& folder, size_t from, size_t to) {
    std::vector<std::wstring> names = FileNames(LoadItems(folder, 0));
    if (from >= names.size() || to >= names.size() || from == to) return;
    MoveName(names, from, to);
    WriteOrder(folder, names);
    RefreshPreview(folder);
}

// ---- Name prompt ----------------------------------------------------------

INT_PTR CALLBACK NameDialogProc(HWND dialog, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INITDIALOG) {
        SetWindowLongPtrW(dialog, DWLP_USER, lParam);
        SetDlgItemTextW(dialog, kNameEditId, reinterpret_cast<std::wstring*>(lParam)->c_str());
        SendDlgItemMessageW(dialog, kNameEditId, EM_SETSEL, 0, -1);
        SetForegroundWindow(dialog);
        return TRUE;
    }
    if (msg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {
        if (LOWORD(wParam) == IDOK) {
            wchar_t text[MAX_PATH];
            GetDlgItemTextW(dialog, kNameEditId, text, MAX_PATH);
            *reinterpret_cast<std::wstring*>(GetWindowLongPtrW(dialog, DWLP_USER)) = text;
        }
        EndDialog(dialog, LOWORD(wParam));
        return TRUE;
    }
    return FALSE;
}

// In-memory DLGTEMPLATE: header, then DWORD-aligned items.
std::vector<WORD> NameDialogTemplate() {
    std::vector<WORD> t;
    auto dword = [&](DWORD v) { t.insert(t.end(), {LOWORD(v), HIWORD(v)}); };
    auto text = [&](PCWSTR s) { t.insert(t.end(), s, s + wcslen(s) + 1); };
    auto item = [&](DWORD style, WORD x, WORD y, WORD cx, WORD cy, WORD id, WORD cls,
                    PCWSTR caption) {
        if (t.size() % 2) t.push_back(0);
        dword(style | WS_CHILD | WS_VISIBLE);
        dword(0);
        t.insert(t.end(), {x, y, cx, cy, id, 0xFFFF, cls});
        text(caption);
        t.push_back(0);  // no creation data
    };
    dword(DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU);
    dword(0);
    t.insert(t.end(), {4, 0, 0, 200, 58, 0, 0});  // items, x, y, cx, cy, menu, class
    text(L"폴더 이름 바꾸기");
    t.push_back(9);
    text(L"Segoe UI");
    item(SS_LEFT, 7, 7, 186, 9, 0xFFFF, 0x0082, L"폴더 이름");
    item(ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP, 7, 18, 186, 13, kNameEditId, 0x0081, L"");
    item(BS_DEFPUSHBUTTON | WS_TABSTOP, 89, 38, 50, 14, IDOK, 0x0080, L"확인");
    item(BS_PUSHBUTTON | WS_TABSTOP, 143, 38, 50, 14, IDCANCEL, 0x0080, L"취소");
    return t;
}

// `name` holds the current name in and the new name out.
bool PromptName(HWND owner, std::wstring* name) {
    std::vector<WORD> t = NameDialogTemplate();
    return DialogBoxIndirectParamW(nullptr, reinterpret_cast<LPCDLGTEMPLATEW>(t.data()),
                                   owner, NameDialogProc,
                                   reinterpret_cast<LPARAM>(name)) == IDOK;
}

// Returns the folder's new path, or empty if it kept its name.
std::wstring RenameFolderTo(const std::wstring& folder, const std::wstring& name) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    if (!manager || g_unloading || !IsValidFolderName(name)) return {};
    std::wstring renamed = Root() + L"\\" + name;
    if (_wcsicmp(renamed.c_str(), folder.c_str()) == 0 ||
        !MoveFileW(folder.c_str(), renamed.c_str()))
        return {};
    IconLocation icon = PinIcon(folder);
    if (IsPreviewIcon(icon)) icon = {MakePreviewIcon(renamed)};
    if (!RepinFolder(manager.Get(), folder, renamed, icon)) {  // the pin keeps the old name
        MoveFileW(renamed.c_str(), folder.c_str());
        DeletePreviewIcons(renamed, L"");
        return {};
    }
    DeleteFileW(PinLnkPath(folder).c_str());
    DeletePreviewIcons(folder, L"");
    Regroup(FolderKeys(renamed));  // the folder's app ID changed with its path
    return renamed;
}

// The folder leaves the taskbar but stays on disk; its apps' windows get their own buttons.
void UnpinFolder(const std::wstring& folder) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (!manager || index < 0 || g_unloading) return;
    Unpin(manager.Get(), pins[index].pidl.get());
    Regroup(FolderKeys(folder));
}

// The folder's shortcuts go back onto the taskbar in its place, in the folder's order,
// then the folder leaves the taskbar for the Recycle Bin. An app pinned already keeps
// the pin it has.
void DissolveFolder(const std::wstring& folder) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (!manager || index < 0 || g_unloading) return;
    std::vector<std::wstring> released = FolderKeys(folder);
    int at = index + 1;
    for (const MenuItem& item : LoadItems(folder, 0))
        if (IsShortcut(item.path) && PinAt(manager.Get(), PinnablePidl(item.path).get(), at)) at++;
    pins = ReadPins();
    index = FindFolderPin(pins, folder);
    if (index >= 0) Unpin(manager.Get(), pins[index].pidl.get());
    RemoveFolder(folder);
    Regroup(released);
}

// The folder and its shortcuts leave the taskbar, the folder for the Recycle Bin; its apps
// aren't pinned again (their windows get buttons of their own).
void DeleteFolder(const std::wstring& folder) {
    ComPtr<IPinManagerInterop3> manager = PinManager();
    std::vector<Pin> pins = ReadPins();
    int index = FindFolderPin(pins, folder);
    if (!manager || index < 0 || g_unloading) return;
    std::vector<std::wstring> released = FolderKeys(folder);
    Unpin(manager.Get(), pins[index].pidl.get());
    RemoveFolder(folder);
    Regroup(released);
}

void RenameFolder(HWND owner, const std::wstring& folder) {
    std::wstring name = FolderName(folder);
    if (PromptName(owner, &name)) RenameFolderTo(folder, name);
}

void PickFolderIcon(HWND owner, const std::wstring& folder) {
    IconLocation icon = PinIcon(folder);
    wchar_t path[MAX_PATH];
    if (IsPreviewIcon(icon)) {
        ExpandEnvironmentStringsW(L"%SystemRoot%\\System32\\imageres.dll", path, MAX_PATH);
        icon.index = 0;
    } else {
        lstrcpynW(path, icon.path.c_str(), MAX_PATH);
    }
    if (PickIconDlg(owner, path, MAX_PATH, &icon.index) && !g_unloading &&
        SetFolderIcon(folder, {path, icon.index}))
        DeletePreviewIcons(folder, L"");
}

// ---- Folder menu ----------------------------------------------------------

// Lets a folder item be dragged out of the menu. The drop is handled here, so
// nothing else ever receives it.
struct DropSource : IDropSource {
    POINT release{};
    bool dropped = false;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** out) override {
        *out = iid == IID_IUnknown || iid == IID_IDropSource ? this : nullptr;
        return *out ? S_OK : E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 2; }  // lives on the stack
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD keys) override {
        if (escape || g_unloading) return DRAGDROP_S_CANCEL;
        if (keys & MK_LBUTTON) return S_OK;
        dropped = GetCursorPos(&release);
        return DRAGDROP_S_CANCEL;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override {
        return DRAGDROP_S_USEDEFAULTCURSORS;
    }
};

enum class MenuDrop { None, Reorder, TakeOut, Move };

struct MenuSession {
    std::wstring folder;
    std::vector<MenuItem> items;
    HMENU menu = nullptr;
    MenuDrop drop = MenuDrop::None;
    UINT from = 0, to = 0;
    std::wstring target;  // Move: the app ID of the folder button it went onto
};

std::wstring FolderButtonAt(POINT pt);
void MoveToFolderButton(const std::wstring& folder, const std::wstring& item, const std::wstring& id);

// Drags items[from]; decides what the drop means once the button is released.
void DragMenuItem(HWND owner, MenuSession& s, UINT from) {
    ComPtr<IShellItem> item;
    ComPtr<IDataObject> data;
    if (FAILED(SHCreateItemFromParsingName(s.items[from].path.c_str(), nullptr,
                                           IID_PPV_ARGS(item.GetAddressOf()))) ||
        FAILED(item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(data.GetAddressOf()))))
        return;
    DropSource source;
    DWORD effect = 0;
    SHDoDragDrop(owner, data.Get(), &source, DROPEFFECT_MOVE | DROPEFFECT_LINK, &effect);
    if (!source.dropped) return;

    wchar_t cls[16] = L"";
    GetClassNameW(WindowFromPoint(source.release), cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"#32768") != 0) {  // outside the menu: onto another folder, or out
        std::wstring id = FolderButtonAt(source.release);
        bool other = !id.empty() && _wcsicmp(id.c_str(), FolderAppId(s.folder).c_str()) != 0;
        s.drop = other ? MenuDrop::Move : MenuDrop::TakeOut;
        s.target = other ? id : L"";
    } else if (int to = MenuItemFromPoint(nullptr, s.menu, source.release);
               to >= 0 && UINT(to) < s.items.size() && UINT(to) != from) {
        s.drop = MenuDrop::Reorder;
        s.to = to;
    }
    s.from = from;
}

LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* s = reinterpret_cast<MenuSession*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (s && msg == WM_MENUDRAG && reinterpret_cast<HMENU>(lParam) == s->menu &&
        wParam < s->items.size()) {
        DragMenuItem(hwnd, *s, wParam);
        return s->drop == MenuDrop::None ? MND_CONTINUE : MND_ENDMENU;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void BuildMenu(MenuSession& s, int iconSize) {
    s.items = LoadItems(s.folder, iconSize);
    s.menu = CreatePopupMenu();
    MENUINFO info{sizeof(info), MIM_STYLE, MNS_DRAGDROP};
    SetMenuInfo(s.menu, &info);
    for (UINT i = 0; i < s.items.size(); i++) {
        std::wstring label = MenuLabel(s.items[i].label);  // the menu copies it
        MENUITEMINFOW mii{sizeof(mii)};
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
        mii.wID = i + 1;
        mii.dwTypeData = label.data();
        mii.hbmpItem = s.items[i].icon;
        InsertMenuItemW(s.menu, i, TRUE, &mii);
    }
    if (!s.items.empty()) AppendMenuW(s.menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(s.menu, MF_STRING, kRenameId, L"이름 바꾸기…");
    AppendMenuW(s.menu, MF_STRING, kIconId, L"아이콘 바꾸기…");
}

// Returns true when the menu should open again to show the change.
bool RunCommand(HWND owner, const MenuSession& s, UINT cmd) {
    switch (s.drop) {
        case MenuDrop::Reorder:
            ReorderItem(s.folder, s.from, s.to);
            return true;
        case MenuDrop::TakeOut:
            TakeOut(s.folder, s.items[s.from].path);
            return GetFileAttributesW(s.folder.c_str()) != INVALID_FILE_ATTRIBUTES;
        case MenuDrop::Move:
            MoveToFolderButton(s.folder, s.items[s.from].path, s.target);
            return GetFileAttributesW(s.folder.c_str()) != INVALID_FILE_ATTRIBUTES;
        case MenuDrop::None:
            break;
    }
    if (cmd == kRenameId)
        RenameFolder(owner, s.folder);
    else if (cmd == kIconId)
        PickFolderIcon(owner, s.folder);
    else if (cmd >= 1 && cmd <= s.items.size())
        OpenOrSwitch(owner, s.items[cmd - 1].path);
    return false;
}

// Runs `work` with a hidden owner window on this thread, for menus and dialogs;
// unload closes what it owns.
void WithOwnerWindow(const std::function<void(HWND)>& work, POINT pt = {}) {
    HWND owner = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", nullptr, WS_POPUP, pt.x, pt.y, 0,
                                 0, nullptr, nullptr, nullptr, nullptr);
    if (!owner) return;
    SetWindowLongPtrW(owner, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(OwnerProc));
    AcquireSRWLockExclusive(&g_lock);
    g_menuOwners.push_back(owner);
    ReleaseSRWLockExclusive(&g_lock);
    SetForegroundWindow(owner);
    work(owner);
    AcquireSRWLockExclusive(&g_lock);
    std::erase(g_menuOwners, owner);
    ReleaseSRWLockExclusive(&g_lock);
    DestroyWindow(owner);
}

void NoteMenuClosed(const std::wstring& folder) {
    AcquireSRWLockExclusive(&g_lock);
    g_lastFolder = folder;
    g_lastCloseTick = GetTickCount64();
    ReleaseSRWLockExclusive(&g_lock);
}

// The click that closes a folder's menu or popup also reaches its button; that one
// must not open it again.
bool JustClosed(const std::wstring& folder) {
    AcquireSRWLockShared(&g_lock);
    bool closed = _wcsicmp(g_lastFolder.c_str(), folder.c_str()) == 0 &&
                  GetTickCount64() - g_lastCloseTick < kToggleMs;
    ReleaseSRWLockShared(&g_lock);
    return closed;
}

void RunFolderMenuIn(HWND owner, const std::wstring& folder, POINT pt);

void RunFolderMenu(const std::wstring& folder, POINT pt) {
    if (IsLoose(folder)) return;  // no files to list
    // TrackPopupMenuEx needs a foreground owner window on this thread.
    WithOwnerWindow([&](HWND owner) { RunFolderMenuIn(owner, folder, pt); }, pt);
}

void RunFolderMenuIn(HWND owner, const std::wstring& folder, POINT pt) {
    int iconSize = GetSystemMetricsForDpi(SM_CXSMICON, GetDpiForWindow(owner));

    // Open from the taskbar edge rather than under the cursor.
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &monitor);
    UINT flags = TPM_RETURNCMD | TPM_CENTERALIGN;
    if (pt.y >= monitor.rcWork.bottom) {
        pt.y = monitor.rcWork.bottom;
        flags |= TPM_BOTTOMALIGN;
    } else if (pt.y < monitor.rcWork.top) {
        pt.y = monitor.rcWork.top;
    }

    for (bool reopen = true; reopen && !g_unloading;) {
        MenuSession s{folder};
        BuildMenu(s, iconSize);
        SetWindowLongPtrW(owner, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&s));
        SetForegroundWindow(owner);
        UINT cmd = TrackPopupMenuEx(s.menu, flags, pt.x, pt.y, owner, nullptr);
        SetWindowLongPtrW(owner, GWLP_USERDATA, 0);
        DestroyMenu(s.menu);
        NoteMenuClosed(folder);
        reopen = !g_unloading && RunCommand(owner, s, cmd);
        FreeIcons(s.items);
    }
}

// Ends open menus, and the name or icon dialog a menu opened.
void CloseMenus() {
    AcquireSRWLockShared(&g_lock);
    for (HWND owner : g_menuOwners) {
        PostMessageW(owner, WM_CANCELMODE, 0, 0);
        // With no popup this returns the owner itself, which must stay.
        if (HWND dialog = GetWindow(owner, GW_ENABLEDPOPUP); dialog && dialog != owner)
            PostMessageW(dialog, WM_CLOSE, 0, 0);
    }
    ReleaseSRWLockShared(&g_lock);
}

// ---- Worker threads -------------------------------------------------------

struct Job {
    std::function<void()> work;
    HMODULE module;
};

DWORD WINAPI JobThread(void* param) {
    std::unique_ptr<Job> job(static_cast<Job*>(param));
    HMODULE module = job->module;
    HRESULT ole = OleInitialize(nullptr);  // COM plus drag and drop
    // An exception leaving the thread would end explorer.
    try {
        job->work();
    } catch (...) {
        Wh_Log(L"Folder job failed: %08X", winrt::to_hresult());
    }
    if (SUCCEEDED(ole)) OleUninitialize();
    job.reset();
    g_jobs--;
    // The module reference keeps the code mapped for these last lines if unload
    // stopped waiting.
    FreeLibraryAndExitThread(module, 0);
}

// Shell and pin work runs off the taskbar thread, which must never block.
void StartJob(std::function<void()> work) {
    auto* job = new Job{std::move(work), nullptr};
    g_jobs++;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                           reinterpret_cast<LPCWSTR>(&JobThread), &job->module)) {
        if (HANDLE thread = CreateThread(nullptr, 0, JobThread, job, 0, nullptr)) {
            CloseHandle(thread);
            return;
        }
        FreeLibrary(job->module);
    }
    g_jobs--;
    delete job;
}

void OnPinLaunched(const std::wstring& folder) {
    if (JustClosed(folder)) return;
    POINT pt;
    GetCursorPos(&pt);
    StartJob([folder, pt] { RunFolderMenu(folder, pt); });
}

// ---- Taskbar thread ---------------------------------------------------------

// The taskbar's XAML thread queue (under g_lock), noted from the first hook that runs
// there. Jobs hand UI work back through it; unload waits for what is queued.
[[clang::no_destroy]] winrt::Windows::System::DispatcherQueue g_taskbarQueue{nullptr};
std::atomic<bool> g_haveTaskbarQueue;
std::atomic<int> g_taskbarWork;  // queued and not yet run

void NoteTaskbarThread() {
    if (g_haveTaskbarQueue) return;
    auto queue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    AcquireSRWLockExclusive(&g_lock);
    g_taskbarQueue = queue;
    ReleaseSRWLockExclusive(&g_lock);
    g_haveTaskbarQueue = true;
}

// Runs `work` on the taskbar thread; exceptions there would take the taskbar down.
bool RunOnTaskbar(std::function<void()> work, bool evenUnloading = false,
                  winrt::Windows::System::DispatcherQueuePriority priority =
                      winrt::Windows::System::DispatcherQueuePriority::Normal) {
    if (g_unloading && !evenUnloading) return false;
    AcquireSRWLockShared(&g_lock);
    winrt::Windows::System::DispatcherQueue queue = g_taskbarQueue;
    ReleaseSRWLockShared(&g_lock);
    if (!queue) return false;
    // Counts the call until its callable is destroyed: that destructor runs module code too.
    struct Pending {
        Pending() { g_taskbarWork++; }
        ~Pending() { g_taskbarWork--; }
    };
    try {
        return queue.TryEnqueue(
            priority, [work = std::move(work), evenUnloading, pending = std::make_shared<Pending>()] {
                try {
                    if (evenUnloading || !g_unloading) work();
                } catch (...) {
                }
            });
    } catch (...) {
        return false;
    }
}

// ---- Merge motion (Composition, taskbar thread) ----------------------------

// Windows 11 motion: decelerate in, accelerate out, at the taskbar's own durations.
constexpr int kFastMs = 167, kNormalMs = 250, kSlowMs = 333;
constexpr float kPlateFrom = 0.7f;
constexpr float kTargetInPlate = 0.82f;  // the target settles into its tile
constexpr float kDraggedOverPlate = 0.85f;
constexpr float kDraggedOpacity = 0.8f;
constexpr float kBounce = 1.12f;

wux::FrameworkElement FindChild(const wux::DependencyObject& parent, std::wstring_view name,
                                int depth = 3) {
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wux::Media::VisualTreeHelper::GetChild(parent, i);
        if (auto element = child.try_as<wux::FrameworkElement>(); element && element.Name() == name)
            return element;
        if (depth > 0)
            if (auto found = FindChild(child, name, depth - 1)) return found;
    }
    return nullptr;
}

// The part of a button that the motion scales: its icon panel.
wux::FrameworkElement IconPanel(const wux::FrameworkElement& button) {
    auto panel = FindChild(button, L"IconPanel");
    return panel ? panel : button;
}

wuc::CompositionEasingFunction Easing(const wuc::Compositor& c, bool in) {
    if (in) return c.CreateCubicBezierEasingFunction({0, 0}, {0, 1});
    return c.CreateCubicBezierEasingFunction({1, 0}, {1, 1});
}

// Scale and opacity of `visual` towards the given values, from its center.
void Animate(const wuc::Visual& visual, wf::Numerics::float2 size, float scale, float opacity,
             int ms, bool in) {
    auto c = visual.Compositor();
    auto easing = Easing(c, in);
    visual.CenterPoint({size.x / 2, size.y / 2, 0});
    auto grow = c.CreateVector3KeyFrameAnimation();
    grow.InsertKeyFrame(1, {scale, scale, 1}, easing);
    grow.Duration(std::chrono::milliseconds(ms));
    auto fade = c.CreateScalarKeyFrameAnimation();
    fade.InsertKeyFrame(1, opacity, easing);
    fade.Duration(std::chrono::milliseconds(ms));
    visual.StartAnimation(L"Scale", grow);
    visual.StartAnimation(L"Opacity", fade);
}

void Animate(const wux::UIElement& element, float scale, float opacity, int ms, bool in) {
    Animate(wux::Hosting::ElementCompositionPreview::GetElementVisual(element),
            element.ActualSize(), scale, opacity, ms, in);
}

// The folder tile that grows under a merge target, drawn on the button's background.
void ShowPlate(const wux::FrameworkElement& button, bool on) {
    using wux::Hosting::ElementCompositionPreview;
    auto background = FindChild(button, L"BackgroundElement");
    if (!background) return;
    auto host = ElementCompositionPreview::GetElementVisual(background);
    auto child = ElementCompositionPreview::GetElementChildVisual(background);
    if (child && child.Comment() != kAppIdPrefix) return;  // someone else's visual
    auto plate = child.try_as<wuc::SpriteVisual>();
    if (!on) {
        if (plate) Animate(plate, background.ActualSize(), 0.8f, 0, kFastMs, false);
        return;
    }
    if (!plate) {
        auto c = host.Compositor();
        plate = c.CreateSpriteVisual();
        plate.Comment(kAppIdPrefix);  // marks the plate as ours
        bool light = button.ActualTheme() == wux::ElementTheme::Light;
        plate.Brush(c.CreateColorBrush(light ? winrt::Windows::UI::Color{0x24, 0, 0, 0}
                                             : winrt::Windows::UI::Color{0x38, 255, 255, 255}));
        plate.RelativeSizeAdjustment({1, 1});
        float radius = 4;
        if (auto border = background.try_as<wux::Controls::Border>())
            radius = std::max(radius, float(border.CornerRadius().TopLeft));
        auto shape = c.CreateRoundedRectangleGeometry();
        shape.CornerRadius({radius, radius});
        auto follow = c.CreateExpressionAnimation(L"host.Size");
        follow.SetReferenceParameter(L"host", host);
        shape.StartAnimation(L"Size", follow);
        plate.Clip(c.CreateGeometricClip(shape));
        plate.Scale({kPlateFrom, kPlateFrom, 1});
        plate.Opacity(0);
        ElementCompositionPreview::SetElementChildVisual(background, plate);
    }
    Animate(plate, background.ActualSize(), 1, 1, kNormalMs, true);
}

// Buttons whose look the motion changed. The taskbar recycles button elements, so
// each one goes back to normal once the merge is over.
[[clang::no_destroy]] std::vector<winrt::weak_ref<wux::FrameworkElement>> g_touched;

void Touch(const wux::FrameworkElement& button) {
    for (auto& weak : g_touched)
        if (weak.get() == button) return;
    g_touched.push_back(winrt::make_weak(button));
}

void ResetButton(const wux::FrameworkElement& button, bool animate) {
    using wux::Hosting::ElementCompositionPreview;
    auto panel = IconPanel(button);
    if (animate) {
        Animate(panel, 1, 1, kFastMs, true);
        ShowPlate(button, false);
        return;
    }
    auto visual = ElementCompositionPreview::GetElementVisual(panel);
    visual.StopAnimation(L"Scale");
    visual.StopAnimation(L"Opacity");
    visual.Scale({1, 1, 1});
    visual.Opacity(1);
    auto background = FindChild(button, L"BackgroundElement");
    if (!background) return;
    auto plate = ElementCompositionPreview::GetElementChildVisual(background);
    if (plate && plate.Comment() == kAppIdPrefix)
        ElementCompositionPreview::SetElementChildVisual(background, nullptr);
}

// `animate`: ease everything back and keep the list for a later snap; otherwise snap
// back now and forget.
void ResetTouched(bool animate) {
    for (auto& weak : g_touched)
        if (auto button = weak.get()) {
            try {
                ResetButton(button, animate);
            } catch (...) {
            }
        }
    if (!animate) g_touched.clear();
}

// A short bounce: the folder took the app.
void Bounce(const wux::FrameworkElement& button) {
    auto panel = IconPanel(button);
    auto visual = wux::Hosting::ElementCompositionPreview::GetElementVisual(panel);
    auto c = visual.Compositor();
    auto size = panel.ActualSize();
    visual.CenterPoint({size.x / 2, size.y / 2, 0});
    auto bounce = c.CreateVector3KeyFrameAnimation();
    bounce.InsertKeyFrame(0.4f, {kBounce, kBounce, 1}, Easing(c, true));
    bounce.InsertKeyFrame(1, {1, 1, 1}, Easing(c, true));
    bounce.Duration(std::chrono::milliseconds(kSlowMs));
    visual.StartAnimation(L"Scale", bounce);
}

// ---- Drag to merge (taskbar thread) -----------------------------------------

struct DragState {
    DWORD thread;
    winrt::weak_ref<wux::FrameworkElement> dragged;
    std::wstring draggedId;
    winrt::weak_ref<wux::FrameworkElement> target;
    std::wstring targetId;
    wux::DispatcherTimer timer{nullptr};
    bool armed = false;  // the merge look is on; a drop now merges
    // Drop placeholder bounds: the last free ones, and whether they are held.
    wf::Rect lastBounds{};
    bool hasLastBounds = false;
    bool frozen = false;
    int updates = 0;  // for the first-update log line
};
// Exists only while a taskbar button is dragged.
[[clang::no_destroy]] std::optional<DragState> g_drag;
// The taskbar's button row, kept from the last drag to find buttons after a merge.
[[clang::no_destroy]] winrt::weak_ref<wux::DependencyObject> g_buttonRow;

DragState* CurrentDrag() {
    return g_drag && g_drag->thread == GetCurrentThreadId() ? &*g_drag : nullptr;
}

wux::FrameworkElement ButtonFromThis(void* pThis) {
    wux::FrameworkElement element{nullptr};
    HRESULT hr = ((IUnknown*)pThis + 3)
                     ->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                      winrt::put_abi(element));
    if (element && winrt::get_class_name(element) == kTaskListButtonClass) return element;
    Wh_Log(L"drag start: no button (hr=%08X class=%s)", hr,
           element ? winrt::get_class_name(element).c_str() : L"-");
    return nullptr;
}

// The app ID in a taskbar button's automation ID ("Appid: <id>"), or empty.
std::wstring_view AppIdFromAutomationId(std::wstring_view id) {
    constexpr std::wstring_view kPrefix = L"Appid: ";
    return id.starts_with(kPrefix) ? id.substr(kPrefix.size()) : std::wstring_view{};
}

std::wstring ButtonAppId(const wux::FrameworkElement& button, bool log = true) {
    std::wstring id{wux::Automation::AutomationProperties::GetAutomationId(button)};
    std::wstring_view appId = AppIdFromAutomationId(id);
    if (appId.empty() && log) Wh_Log(L"button without app id: '%s'", id.c_str());
    return std::wstring(appId);
}

// Where the cursor is on a neighbour button. The taskbar swaps as soon as the dragged
// icon's edge overlaps a neighbour, which would move it away before the cursor gets
// to its middle; so the neighbour holds still until the cursor reaches its far quarter.
enum class Zone { Free, Hold, Merge };

// `fromNear`: cursor distance from the neighbour's edge facing the dragged slot, as a
// share of its width.
Zone ZoneAt(float fromNear) {
    if (fromNear < 0 || fromNear > 1 - kMergeEdge) return Zone::Free;  // far quarter swaps
    return fromNear >= kMergeEdge ? Zone::Merge : Zone::Hold;
}

struct Hover {
    wux::FrameworkElement button{nullptr};
    Zone zone = Zone::Free;
};

// The button under `point` (repeater coordinates), other than `dragged`.
Hover HoverAt(const wux::FrameworkElement& dragged, wf::Point point) {
    auto repeater = wux::Media::VisualTreeHelper::GetParent(dragged);
    int count = repeater ? wux::Media::VisualTreeHelper::GetChildrenCount(repeater) : 0;
    float slot = dragged.ActualOffset().x;  // the drop placeholder's place
    for (int i = 0; i < count; i++) {
        auto child = wux::Media::VisualTreeHelper::GetChild(repeater, i)
                         .try_as<wux::FrameworkElement>();
        if (!child || child == dragged || winrt::get_class_name(child) != kTaskListButtonClass)
            continue;
        auto offset = child.ActualOffset();  // layout slot, unaffected by our scale
        float width = child.ActualWidth(), height = child.ActualHeight();
        if (width <= 0 || point.X < offset.x || point.X > offset.x + width ||
            point.Y < offset.y || point.Y > offset.y + height)
            continue;
        float share = (point.X - offset.x) / width;
        return {child, ZoneAt(offset.x < slot ? 1 - share : share)};
    }
    return {};
}

// ---- Folder popup (XAML flyout, taskbar thread) -----------------------------

// The folder's apps in one row, on glass like the taskbar's. The flyout lives on the
// taskbar's own XAML thread, so it gets the system's theme and motion.

// A cell holds the icon and its name when names show, centred; `spacing` separates
// cells. Without names a cell is a square around the icon.
struct CellSize {
    double width, height;
};
CellSize CellOf(const Look& look) {
    double icon = look.iconSize;
    return look.labels ? CellSize{std::max(80.0, icon + 48), icon + 44}
                       : CellSize{icon + 22, icon + 22};
}
double Pitch(const Look& look) {
    return CellOf(look).width + look.spacing;
}
constexpr double kDragStart = 4;  // DIPs the pointer moves before a press becomes a drag
constexpr double kBubbleSlack = 8;  // DIPs a drag may stray past the bubble and stay in it

// One corner of a rounded rectangle with Figma's corner smoothing, the continuous curve
// of Apple's icons: `smoothing` 0 is a circular arc, 0.6 is iOS. The smoothing is cut
// back to fit `budget` (half the side).
struct SmoothCorner {
    double a, b, c, d, p, arc, radius;
};
SmoothCorner SmoothCornerOf(double radius, double smoothing, double budget) {
    radius = std::clamp(radius, 0.0, budget);
    if (radius <= 0) return {};
    smoothing = std::clamp(smoothing, 0.0, std::max(0.0, budget / radius - 1));
    constexpr double kDegree = std::numbers::pi / 180;
    double p = std::min((1 + smoothing) * radius, budget);
    double arcMeasure = 90 * (1 - smoothing);
    double arc = std::sin(arcMeasure / 2 * kDegree) * radius * std::sqrt(2.0);
    double alpha = (90 - arcMeasure) / 2;
    double beta = 45 * smoothing;
    double c = radius * std::tan(alpha / 2 * kDegree) * std::cos(beta * kDegree);
    double d = c * std::tan(beta * kDegree);
    double b = (p - arc - c - d) / 3;
    return {2 * b, b, c, d, p, arc, radius};
}

// The outline of a `w` by `h` rectangle at `x, y` with smooth corners.
wux::Media::PathGeometry SmoothRect(double x, double y, double w, double h, double radius,
                                    double smoothing) {
    SmoothCorner k = SmoothCornerOf(radius, smoothing, std::max(0.0, std::min(w, h) / 2));
    // Each corner starts where its edge stops being straight, runs along `u` and turns
    // toward `v`, and ends `p` along and `p` in.
    struct Corner {
        double x, y, ux, uy, vx, vy;
    };
    const Corner corners[] = {{x + w - k.p, y, 1, 0, 0, 1},
                              {x + w, y + h - k.p, 0, 1, -1, 0},
                              {x + k.p, y + h, -1, 0, 0, -1},
                              {x, y + k.p, 0, -1, 1, 0}};
    wux::Media::PathFigure figure;
    figure.IsClosed(true);
    figure.StartPoint({float(corners[0].x), float(corners[0].y)});
    auto segments = figure.Segments();
    for (const Corner& corner : corners) {
        auto at = [&](double along, double in) {
            return wf::Point{float(corner.x + corner.ux * along + corner.vx * in),
                             float(corner.y + corner.uy * along + corner.vy * in)};
        };
        if (&corner != &corners[0]) {
            wux::Media::LineSegment edge;
            edge.Point(at(0, 0));
            segments.Append(edge);
        }
        wux::Media::BezierSegment into;
        into.Point1(at(k.a, 0));
        into.Point2(at(k.a + k.b, 0));
        into.Point3(at(k.a + k.b + k.c, k.d));
        wux::Media::ArcSegment turn;
        turn.Size({float(k.radius), float(k.radius)});
        turn.SweepDirection(wux::Media::SweepDirection::Clockwise);
        turn.Point(at(k.p - k.d, k.d + k.arc));
        wux::Media::BezierSegment outOf;
        outOf.Point1(at(k.p, k.d + k.arc + k.c));
        outOf.Point2(at(k.p, k.d + k.arc + k.b + k.c));
        outOf.Point3(at(k.p, k.p));
        segments.Append(into);
        segments.Append(turn);
        segments.Append(outOf);
    }
    wux::Media::PathGeometry geometry;
    geometry.Figures().Append(figure);
    return geometry;
}

struct PopupItem {
    std::wstring path, fileName, label;
    Pixels icon;
    int iconSize = 0;
    bool running = false;
    LooseWindow loose;  // the app, in the loose folder
};

struct PopupModel {
    std::wstring folder;
    std::vector<PopupItem> items;
};

// An app's right-click menu in a folder, read on a worker.
struct AppMenuModel {
    std::wstring folder;
    PopupItem app;
    bool shortcut = false;  // only shortcuts can go back to the taskbar
    std::vector<Document> documents;
    std::vector<HWND> windows;
};

// The loose folder's apps, one cell per app in the order they came.
PopupModel LoadLooseModel(int iconSize) {
    PopupModel model{kLooseFolder};
    std::vector<LooseWindow> apps;
    AcquireSRWLockShared(&g_lock);
    for (const auto& [window, loose] : g_running.loose) {
        if (!IsWindow(window) || !IsWindowVisible(window)) continue;
        auto it = std::find_if(apps.begin(), apps.end(),
                               [&](const LooseWindow& app) { return app.key == loose.key; });
        if (it == apps.end())
            apps.push_back(loose);
        else if (loose.order < it->order)
            *it = loose;
    }
    ReleaseSRWLockShared(&g_lock);
    std::sort(apps.begin(), apps.end(),
              [](const LooseWindow& a, const LooseWindow& b) { return a.order < b.order; });
    for (LooseWindow& app : apps) {
        PopupItem item;
        ComPtr<IShellItem> shell = LooseShellItem(app);
        if (shell) item.label = AppLabel(shell.Get());
        if (item.label.empty()) item.label = app.appId;
        if (IconPixels(shell.Get(), iconSize, &item.icon)) item.iconSize = iconSize;
        item.loose = std::move(app);
        model.items.push_back(std::move(item));
    }
    return model;
}

AppMenuModel LoadAppMenu(const std::wstring& folder, PopupItem app, int iconSize) {
    AppMenuModel model{folder, std::move(app)};
    if (IsLoose(folder)) {  // a running app by its own app ID, with no shortcut
        model.documents = AppDocuments(model.app.loose.appId, iconSize, 8);
        model.windows = LooseWindows(model.app.loose.key);
        return model;
    }
    model.shortcut = IsShortcut(model.app.path);
    std::wstring appId = ItemAppId(folder, model.app.fileName);
    model.documents = AppDocuments(appId, iconSize, 8);
    model.windows = ItemWindows(folder, model.app.fileName);
    Wh_Log(L"app menu %s (%s): %d documents, %d windows", model.app.fileName.c_str(),
           appId.c_str(), int(model.documents.size()), int(model.windows.size()));
    return model;
}

// Read on a worker: names, icon pixels and which apps run.
PopupModel LoadPopupModel(const std::wstring& folder, int iconSize) {
    if (IsLoose(folder)) return LoadLooseModel(iconSize);
    PopupModel model{folder};
    for (MenuItem& item : LoadItems(folder, 0)) {
        PopupItem popupItem{item.path, item.fileName, item.label};
        if (IconPixels(item.path, iconSize, &popupItem.icon)) popupItem.iconSize = iconSize;
        popupItem.running = !ItemWindows(folder, item.fileName).empty();
        model.items.push_back(std::move(popupItem));
    }
    return model;
}

// Slot of the point `x` (canvas DIPs) in a row of `count` cells.
size_t SlotAt(double x, double cellWidth, size_t count) {
    return std::clamp<size_t>(x > 0 ? size_t(x / cellWidth) : 0, 0, count ? count - 1 : 0);
}

// Where item `i` shows while the item at `from` is held over slot `to`.
size_t PreviewSlot(size_t i, size_t from, size_t to) {
    if (i == from) return to;
    if (from < to && i > from && i <= to) return i - 1;
    if (to < from && i >= to && i < from) return i + 1;
    return i;
}

wf::Point SlotPosition(size_t slot, double pitch) {
    return {float(slot * pitch), 0};
}

wux::Media::Imaging::WriteableBitmap ToBitmap(const Pixels& pixels, int size) {
    if (!size) return nullptr;
    wux::Media::Imaging::WriteableBitmap bitmap(size, size);
    auto buffer = bitmap.PixelBuffer();
    winrt::com_ptr<::Windows::Storage::Streams::IBufferByteAccess> access;
    BYTE* bytes = nullptr;
    if (buffer.Capacity() < pixels.size() * 4 ||
        FAILED(reinterpret_cast<::IUnknown*>(winrt::get_abi(buffer))
                   ->QueryInterface(winrt::guid_of<::Windows::Storage::Streams::IBufferByteAccess>(),
                                    access.put_void())) ||
        FAILED(access->Buffer(&bytes)))
        return nullptr;
    memcpy(bytes, pixels.data(), pixels.size() * 4);
    buffer.Length(pixels.size() * 4);
    bitmap.Invalidate();
    return bitmap;
}

wux::Media::SolidColorBrush Brush(uint32_t argb) {
    return wux::Media::SolidColorBrush(winrt::Windows::UI::Color{
        BYTE(argb >> 24), BYTE(argb >> 16), BYTE(argb >> 8), BYTE(argb)});
}

// HeroUI v3's default theme (@heroui/styles): layers of fill rather than lines, large
// corners. The surface is glass like the taskbar's, tinted with the theme's surface
// (dark lifted to a middle gray), and fills on it are see-through layers. The one
// accent is pink (#FF7A9E dark, #E04E78 light).
struct Palette {
    uint32_t surface, border, hover, press, field, text, muted, accent;
    uint32_t running;  // the accent, soft: under a running app
    double tintOpacity, tintLuminosity;
};
constexpr Palette kDarkPalette{0xFF1A1A1D, 0x18FFFFFF, 0x14FFFFFF, 0x0CFFFFFF, 0x14FFFFFF,
                               0xFFFCFCFC, 0xFFA1A1AA, 0xFFFF7A9E, 0x30FF7A9E, 0.55, 0.8};
constexpr Palette kLightPalette{0xFFFFFFFF, 0x14000000, 0x0F000000, 0x08000000, 0x0F000000,
                                0xFF18181B, 0xFF71717A, 0xFFE04E78, 0x24E04E78, 0.55, 0.85};
constexpr double kCellRadius = 14, kFieldRadius = 12, kMenuItemRadius = 10;

// Overrides the brushes a control's template looks up (XAML lightweight styling).
void SetBrushes(const wux::FrameworkElement& element,
                std::initializer_list<std::pair<const wchar_t*, uint32_t>> brushes) {
    auto resources = element.Resources();
    for (const auto& [key, argb] : brushes) resources.Insert(winrt::box_value(key), Brush(argb));
}

// The surface as glass; with pill.glass off, the surface color alone.
wux::Media::Brush GlassBrush(const Palette& colors) {
    if (!settings::g.pill.glass) return Brush(colors.surface);
    auto color = Brush(colors.surface).Color();
    wux::Media::AcrylicBrush glass;
    glass.BackgroundSource(wux::Media::AcrylicBackgroundSource::HostBackdrop);
    glass.TintColor(color);
    glass.TintOpacity(colors.tintOpacity);
    glass.TintLuminosityOpacity(colors.tintLuminosity);
    glass.FallbackColor(color);
    return glass;
}

// A popup window draws the desktop behind it only once DWM lets it; the taskbar's other
// popups on this thread may take it too.
void AllowHostBackdrop() {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM) -> BOOL {
            wchar_t name[32];
            if (GetClassNameW(window, name, ARRAYSIZE(name)) &&
                wcscmp(name, L"Xaml_WindowedPopupClass") == 0) {
                BOOL allow = TRUE;
                DwmSetWindowAttribute(window, 17 /* DWMWA_USE_HOSTBACKDROPBRUSH */, &allow,
                                      sizeof(allow));
            }
            return TRUE;
        },
        0);
}

// The flyout's own presenter shows nothing: the bubble inside draws the glass.
wux::Style BarePresenter() {
    wux::Style style{winrt::xaml_typename<wux::Controls::FlyoutPresenter>()};
    auto setters = style.Setters();
    auto set = [&](const wux::DependencyProperty& property, const wf::IInspectable& value) {
        setters.Append(wux::Setter(property, value));
    };
    set(wux::Controls::Control::PaddingProperty(), winrt::box_value(wux::Thickness{}));
    set(wux::Controls::Control::BackgroundProperty(), Brush(0));
    set(wux::Controls::Control::BorderThicknessProperty(), winrt::box_value(wux::Thickness{}));
    set(wux::Controls::Control::CornerRadiusProperty(), winrt::box_value(wux::CornerRadius{}));
    set(wux::Controls::FlyoutPresenter::IsDefaultShadowEnabledProperty(), winrt::box_value(false));
    set(wux::FrameworkElement::MinWidthProperty(), winrt::box_value(0.0));
    set(wux::FrameworkElement::MinHeightProperty(), winrt::box_value(0.0));
    set(wux::FrameworkElement::MaxWidthProperty(), winrt::box_value(10000.0));
    set(wux::FrameworkElement::MaxHeightProperty(), winrt::box_value(10000.0));
    return style;
}

// One popup at a time: the folder's apps, its right-click menu, or its name field.
struct Popup {
    PopupModel model;
    Look look;
    const Palette* colors = &kDarkPalette;
    wux::Controls::Primitives::FlyoutBase flyout{nullptr};
    wux::FrameworkElement content{nullptr};  // the bubble
    wux::Shapes::Path body{nullptr};          // its glass, maybe a brush of this module
    wux::Controls::TextBox name{nullptr};
    wux::Controls::Canvas canvas{nullptr};
    std::vector<wux::Controls::Grid> cells;
    // The press or drag on a cell.
    bool down = false, dragging = false;
    size_t from = 0, to = 0;
    wf::Point start{};       // canvas DIPs at the press
    RECT contentOnScreen{};  // the bubble in screen pixels, for drops outside
    int slack = 0;           // kBubbleSlack in screen pixels
    winrt::weak_ref<wux::FrameworkElement> anchor;  // the folder's button
    bool taskbarOnTop = false;
    HWND taskbar = nullptr;  // the taskbar window it opened from
    bool selfClose = false;  // hidden by the mod, not dismissed by a press
    std::shared_ptr<void> foreground;  // the foreground hook while it shows
    wux::DispatcherTimer watch{nullptr};  // WatchTick, while it shows
    bool activating = false;  // its taskbar was asked to the front since the pointer came onto it
    // Every handler the popup attached, revoked as these go: XAML raises some events
    // (LostFocus) after the fact, and none may reach a closed popup or unloaded code.
    std::vector<std::shared_ptr<void>> handlers;
};
[[clang::no_destroy]] std::optional<Popup> g_popup;

template <typename Revoker>
void Keep(Popup& p, Revoker revoker) {
    p.handlers.push_back(std::make_shared<Revoker>(std::move(revoker)));
}

// Drops the popup's XAML; none of its handlers runs after this.
void ClosePopup() {
    if (!g_popup) return;
    try {
        g_popup->foreground.reset();
        if (g_popup->watch) g_popup->watch.Stop();
        g_popup->handlers.clear();
        if (g_popup->body) g_popup->body.Fill(nullptr);  // the brush goes now, not with XAML's last reference
        if (g_popup->flyout) {  // null when building it failed
            g_popup->flyout.Hide();
            if (auto flyout = g_popup->flyout.try_as<wux::Controls::Flyout>()) flyout.Content(nullptr);
        }
    } catch (...) {
    }
    g_popup.reset();
}

// Hides the popup on the mod's own account, so its closing switches to no other folder.
void HideSelf(Popup& p) {
    p.selfClose = true;
    p.flyout.Hide();
}

double Smoothing(const Look& look) {
    return look.smoothing / 100.0;
}

void SetPlateFill(const wux::Controls::Grid& plate, uint32_t argb) {
    plate.Children().GetAt(0).as<wux::Shapes::Path>().Fill(Brush(argb));
}

// A `w` by `h` plate in the smooth-cornered shape under `content`, lit under the
// pointer as HeroUI's list items; at rest it shows `rest`.
wux::Controls::Grid Plate(Popup& p, double w, double h, double radius,
                          const wux::UIElement& content, uint32_t rest = 0) {
    wux::Controls::Grid plate;
    plate.Width(w);
    plate.Height(h);
    plate.Background(Brush(0));  // transparent but hit-testable all over
    wux::Shapes::Path fill;
    fill.Data(SmoothRect(0, 0, w, h, radius, Smoothing(p.look)));
    fill.Fill(Brush(rest));
    plate.Children().Append(fill);
    plate.Children().Append(content);
    uint32_t hover = p.colors->hover;
    Keep(p, plate.PointerEntered(winrt::auto_revoke, [hover](const wf::IInspectable& sender, const auto&) {
        if (auto grid = sender.try_as<wux::Controls::Grid>()) SetPlateFill(grid, hover);
    }));
    Keep(p, plate.PointerExited(winrt::auto_revoke, [rest](const wf::IInspectable& sender, const auto&) {
        if (auto grid = sender.try_as<wux::Controls::Grid>()) SetPlateFill(grid, rest);
    }));
    return plate;
}

constexpr double kShadowRoom = 24;  // clear margin around the bubble for its shadow

// The popup's body: glass in the smooth-cornered shape with a soft shadow, holding
// `content` inset by `padding`. With the pill on it is the pill's glass, as set now.
wux::FrameworkElement BuildBubble(Popup& p, const wux::FrameworkElement& content,
                                  double padding) {
    wux::Controls::Grid shell;
    wux::Controls::Border shadowHost;
    wux::Shapes::Path body;
    bool pillGlass = settings::g.pill.enabled;
    bool rim = pillGlass && settings::g.pill.style == settings::Style::kGlass;
    if (pillGlass) {
        // The tint resolves in the body's theme; the popup's own may not be the taskbar's.
        body.RequestedTheme(p.colors == &kLightPalette ? wux::ElementTheme::Light
                                                       : wux::ElementTheme::Dark);
        body.Fill(winrt::make<pill::GlassBrush>(body, true));
        if (rim) body.Stroke(pill::Rim());
        body.StrokeThickness(rim ? 1 : 0);  // the rim's pixel is known once the body has a root
    } else {
        body.Fill(GlassBrush(*p.colors));
        body.Stroke(Brush(p.colors->border));
        body.StrokeThickness(1);
    }
    p.body = body;
    content.Margin({padding, padding, padding, padding});
    shell.Children().Append(shadowHost);
    shell.Children().Append(body);
    shell.Children().Append(content);

    auto compositor = wux::Hosting::ElementCompositionPreview::GetElementVisual(shadowHost).Compositor();
    auto sprite = compositor.CreateSpriteVisual();
    auto shadow = compositor.CreateDropShadow();
    shadow.Mask(body.GetAlphaMask());
    shadow.BlurRadius(20);
    shadow.Offset({0, 6, 0});
    shadow.Opacity(p.colors == &kLightPalette ? 0.18f : 0.45f);
    sprite.Shadow(shadow);
    wux::Hosting::ElementCompositionPreview::SetElementChildVisual(shadowHost, sprite);
    double radius = p.look.radius, smoothing = Smoothing(p.look), scale = UiScale(p.look);
    Keep(p, shell.SizeChanged(winrt::auto_revoke, [body, sprite, radius, smoothing, rim, scale](
                                                      const auto&, const wux::SizeChangedEventArgs& args) {
        wf::Size size = args.NewSize();
        // The bubble is drawn scaled, so a physical pixel is that much more of its DIPs.
        if (rim) body.StrokeThickness(pill::PhysicalPixel(body) / scale);
        // Inset by half the stroke, so the line stays inside.
        double inset = body.StrokeThickness() / 2;
        body.Data(SmoothRect(inset, inset, size.Width - 2 * inset, size.Height - 2 * inset, radius,
                             smoothing));
        sprite.Size({size.Width, size.Height});
    }));

    wux::Controls::Grid room;
    room.Padding({kShadowRoom, kShadowRoom, kShadowRoom, kShadowRoom});
    room.Children().Append(shell);
    p.content = shell;
    return room;
}

void LayoutCells(Popup& p) {
    CellSize cell = CellOf(p.look);
    double pitch = Pitch(p.look), inset = p.look.spacing / 2.0;
    for (size_t i = 0; i < p.cells.size(); i++) {
        wf::Point at = SlotPosition(i, pitch);
        wux::Controls::Canvas::SetLeft(p.cells[i], at.X + inset);
        wux::Controls::Canvas::SetTop(p.cells[i], at.Y + inset);
    }
    p.canvas.Width(std::max<size_t>(p.cells.size(), 1) * pitch);
    p.canvas.Height(cell.height + p.look.spacing);
}

// Cells of the held item's neighbours slide to where they would go.
void PreviewReorder(Popup& p, size_t to) {
    if (to == p.to) return;
    p.to = to;
    double pitch = Pitch(p.look);
    for (size_t i = 0; i < p.cells.size(); i++) {
        if (i == p.from) continue;
        wf::Point now = SlotPosition(i, pitch), then = SlotPosition(PreviewSlot(i, p.from, to), pitch);
        p.cells[i].Translation({then.X - now.X, then.Y - now.Y, 0});
    }
}

wux::Vector3Transition Slide() {
    wux::Vector3Transition slide;
    slide.Duration(std::chrono::milliseconds(kFastMs));
    return slide;
}

// Cells go back to their own slots: sliding, or at once when the slots just changed.
void EndCellDrag(Popup& p, bool slide) {
    p.down = p.dragging = false;
    for (auto& cell : p.cells) {
        if (!slide) cell.TranslationTransition(nullptr);
        cell.Translation({0, 0, 0});
        cell.TranslationTransition(Slide());
        wux::Controls::Canvas::SetZIndex(cell, 0);
        cell.Opacity(1);
    }
}

bool OutOfBubble(const Popup& p);

void FinishCellPress(Popup& p) {
    if (!p.down) return;
    std::wstring folder = p.model.folder;
    bool loose = IsLoose(folder);
    if (!p.dragging) {  // a click
        std::wstring path = p.model.items[p.from].path, key = p.model.items[p.from].loose.key;
        EndCellDrag(p, true);
        HideSelf(p);
        if (loose)
            StartJob([key] { SwitchToLoose(key); });
        else
            StartJob([path] { OpenOrSwitch(nullptr, path); });
        return;
    }
    size_t from = p.from, to = p.to;
    if (OutOfBubble(p)) {  // out of the popup: back to the taskbar
        EndCellDrag(p, true);
        std::wstring path = p.model.items[from].path;
        LooseWindow app = p.model.items[from].loose;
        HideSelf(p);
        if (loose)
            StartJob([app] { PinLoose(app); });
        else
            StartJob([folder, path] { TakeOut(folder, path); });
        return;
    }
    // The loose folder keeps its apps in the order they came.
    EndCellDrag(p, to == from || loose);
    if (to == from || loose) return;
    PopupItem moved = std::move(p.model.items[from]);
    p.model.items.erase(p.model.items.begin() + from);
    p.model.items.insert(p.model.items.begin() + to, std::move(moved));
    auto cell = p.cells[from];
    p.cells.erase(p.cells.begin() + from);
    p.cells.insert(p.cells.begin() + to, cell);
    LayoutCells(p);
    StartJob([folder, from, to] { ReorderItem(folder, from, to); });
}

void OpenAppMenu(Popup& p, const wux::Controls::Grid& cell);
bool StartCarry(Popup& p, const wux::Controls::Grid& cell);

// The physical state of the primary mouse button, whatever the thread's input saw.
bool PrimaryButtonDown() {
    return GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000;
}

// The drag of a cell leaves the popup once the pointer is clearly past the bubble.
bool OutOfBubble(const Popup& p) {
    POINT cursor;
    RECT bubble = p.contentOnScreen;
    InflateRect(&bubble, p.slack, p.slack);
    return GetCursorPos(&cursor) && !PtInRect(&bubble, cursor);
}

wux::Controls::Grid BuildCell(Popup& p, size_t index) {
    const PopupItem& item = p.model.items[index];
    const Palette& colors = *p.colors;
    CellSize size = CellOf(p.look);

    wux::Controls::StackPanel stack;
    stack.HorizontalAlignment(wux::HorizontalAlignment::Center);
    stack.VerticalAlignment(wux::VerticalAlignment::Center);
    wux::Controls::Image image;
    image.Width(p.look.iconSize);
    image.Height(p.look.iconSize);
    if (auto bitmap = ToBitmap(item.icon, item.iconSize)) image.Source(bitmap);
    stack.Children().Append(image);
    if (p.look.labels) {
        wux::Controls::TextBlock label;
        label.Text(item.label);
        label.FontSize(12);
        label.Foreground(Brush(colors.text));
        label.MaxWidth(size.width - 8);
        label.Margin({0, 6, 0, 0});
        label.TextAlignment(wux::TextAlignment::Center);
        label.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        stack.Children().Append(label);
    }
    // A running app sits on a soft accent tile instead of a mark of its own.
    auto cell = Plate(p, size.width, size.height, std::min(kCellRadius, size.width * 0.3), stack,
                      item.running ? colors.running : 0);
    cell.TranslationTransition(Slide());
    wux::Automation::AutomationProperties::SetName(cell, item.label);
    if (!p.look.labels)  // the name on hover instead
        wux::Controls::ToolTipService::SetToolTip(cell, winrt::box_value(item.label));

    uint32_t press = colors.press;
    Keep(p, cell.PointerPressed(winrt::auto_revoke, [press](const wf::IInspectable& sender,
                                const wux::Input::PointerRoutedEventArgs& args) {
        auto grid = sender.try_as<wux::Controls::Grid>();
        if (!g_popup || !grid) return;
        Popup& p = *g_popup;
        args.Handled(true);
        if (!args.GetCurrentPoint(grid).Properties().IsLeftButtonPressed()) return;
        auto it = std::find(p.cells.begin(), p.cells.end(), grid);
        if (it == p.cells.end()) return;
        p.down = true;
        p.dragging = false;
        p.from = p.to = it - p.cells.begin();
        p.start = args.GetCurrentPoint(p.canvas).Position();
        // The bubble's screen rectangle, from where the press landed in both.
        POINT cursor;
        GetCursorPos(&cursor);
        wf::Point inContent = args.GetCurrentPoint(p.content).Position();
        double scale = (p.content.XamlRoot() ? p.content.XamlRoot().RasterizationScale() : 1) * UiScale(p.look);
        p.contentOnScreen.left = LONG(cursor.x - inContent.X * scale);
        p.contentOnScreen.top = LONG(cursor.y - inContent.Y * scale);
        p.contentOnScreen.right = LONG(p.contentOnScreen.left + p.content.ActualWidth() * scale);
        p.contentOnScreen.bottom = LONG(p.contentOnScreen.top + p.content.ActualHeight() * scale);
        p.slack = int(std::lround(kBubbleSlack * scale));
        SetPlateFill(grid, press);
        grid.CapturePointer(args.Pointer());
    }));
    Keep(p, cell.PointerMoved(winrt::auto_revoke, [](const wf::IInspectable& sender,
                         const wux::Input::PointerRoutedEventArgs& args) {
        auto grid = sender.try_as<wux::Controls::Grid>();
        if (!g_popup || !g_popup->down || !grid) return;
        Popup& p = *g_popup;
        wf::Point at = args.GetCurrentPoint(p.canvas).Position();
        float dx = at.X - p.start.X, dy = at.Y - p.start.Y;
        if (!p.dragging && std::hypot(dx, dy) < kDragStart) return;
        if (!p.dragging) {
            p.dragging = true;
            grid.TranslationTransition(nullptr);  // the held cell follows the pointer as is
            wux::Controls::Canvas::SetZIndex(grid, 1);
            grid.Opacity(0.85);
        }
        args.Handled(true);
        grid.Translation({dx, dy, 0});
        // Past the bubble the drag goes on outside, as the app's icon alone.
        if (OutOfBubble(p) && StartCarry(p, grid)) return;
        bool inside = at.X >= 0 && at.Y >= 0 && at.X < p.canvas.ActualWidth() &&
                      at.Y < p.canvas.ActualHeight();
        PreviewReorder(p, inside ? SlotAt(at.X, Pitch(p.look), p.cells.size()) : p.from);
    }));
    Keep(p, cell.PointerReleased(winrt::auto_revoke, [](const wf::IInspectable& sender,
                            const wux::Input::PointerRoutedEventArgs& args) {
        auto grid = sender.try_as<wux::Controls::Grid>();
        if (!g_popup || !grid) return;
        args.Handled(true);
        if (args.GetCurrentPoint(grid).Properties().PointerUpdateKind() ==
            winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased)
            return OpenAppMenu(*g_popup, grid);
        grid.ReleasePointerCapture(args.Pointer());
        FinishCellPress(*g_popup);
    }));
    // Capture can go without a release (the pointer left the popup's window, say); a drag
    // past the bubble with the button still held goes on outside.
    Keep(p, cell.PointerCaptureLost(winrt::auto_revoke, [](const wf::IInspectable& sender, const auto&) {
        if (!g_popup) return;
        if (auto grid = sender.try_as<wux::Controls::Grid>();
            grid && OutOfBubble(*g_popup) && StartCarry(*g_popup, grid))
            return;
        FinishCellPress(*g_popup);
    }));
    return cell;
}

// The folder's name in a filled field whose focus shows as an accent ring, as HeroUI's
// input. Enter or leaving it renames; Esc keeps the name.
wux::Controls::TextBox BuildNameField(Popup& p) {
    const Palette& colors = *p.colors;
    wux::Controls::TextBox name;
    name.Text(FolderName(p.model.folder));
    name.Width(240);
    name.FontSize(15);
    name.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    name.CornerRadius({kFieldRadius, kFieldRadius, kFieldRadius, kFieldRadius});
    name.BorderThickness({2, 2, 2, 2});
    name.Padding({10, 5, 10, 5});
    name.SelectionHighlightColor(Brush(colors.accent));
    SetBrushes(name, {{L"TextControlBackground", colors.field},
                      {L"TextControlBackgroundPointerOver", colors.hover},
                      {L"TextControlBackgroundFocused", colors.field},
                      {L"TextControlBorderBrush", 0},
                      {L"TextControlBorderBrushPointerOver", 0},
                      {L"TextControlBorderBrushFocused", colors.accent},
                      {L"TextControlForeground", colors.text},
                      {L"TextControlForegroundPointerOver", colors.text},
                      {L"TextControlForegroundFocused", colors.text}});
    wux::Automation::AutomationProperties::SetName(name, L"폴더 이름");
    p.name = name;
    auto commit = [](const wux::Controls::TextBox& box) {
        if (!g_popup || g_popup->name != box) return;
        std::wstring folder = g_popup->model.folder, typed{box.Text()};
        if (typed == FolderName(folder)) return;
        if (!IsValidFolderName(typed)) return box.Text(FolderName(folder));
        StartJob([folder, typed] {
            std::wstring renamed = RenameFolderTo(folder, typed);
            RunOnTaskbar([folder, renamed] {
                if (g_popup && g_popup->model.folder == folder && !renamed.empty())
                    g_popup->model.folder = renamed;
            });
        });
    };
    Keep(p, name.KeyDown(winrt::auto_revoke, [commit](const wf::IInspectable& sender, const wux::Input::KeyRoutedEventArgs& args) {
        auto box = sender.try_as<wux::Controls::TextBox>();
        if (!box || !g_popup) return;
        if (args.Key() == winrt::Windows::System::VirtualKey::Escape) {
            box.Text(FolderName(g_popup->model.folder));  // what closing will commit
        } else if (args.Key() == winrt::Windows::System::VirtualKey::Enter) {
            args.Handled(true);
            commit(box);
            HideSelf(*g_popup);
        }
    }));
    Keep(p, name.LostFocus(winrt::auto_revoke, [commit](const wf::IInspectable& sender, const auto&) {
        if (auto box = sender.try_as<wux::Controls::TextBox>()) commit(box);
    }));
    return name;
}

// The taskbar window of the folder button last clicked: every popup starts from one, the
// later ones (an app's menu, the name field) from the popup it opened. Taskbar thread.
HWND g_folderTaskbar = nullptr;

void SwitchFolderOnPress(const Popup& p);
std::wstring ClassOf(HWND window);

// The taskbars and the popup's own windows run on this thread; the desktop, the previews
// and apps do not. Taskbar thread.
bool OnThisThread(HWND window) {
    return GetWindowThreadProcessId(GetAncestor(window, GA_ROOTOWNER), nullptr) == GetCurrentThreadId();
}

// A press outside the taskbar closes the popup through the taskbar's deactivation, which
// XAML misses when the click that opened it took no activation (under the window
// previews); so the popup also closes as the foreground leaves the taskbars.
void CALLBACK OnForeground(HWINEVENTHOOK hook, DWORD, HWND window, LONG, LONG, DWORD, DWORD) {
    try {
        if (g_unloading || OnThisThread(window)) return;
        // Outside whatever XAML call let the event in.
        RunOnTaskbar([hook, name = ClassOf(window)] {
            if (!g_popup || g_popup->foreground.get() != hook || !g_popup->flyout.IsOpen()) return;
            core::DiagLine(L"Popup closes: foreground %s", name.c_str());
            HideSelf(*g_popup);
        });
    } catch (...) {
    }
}

void GiveBackLater();
void StartWatch(Popup& p);
void ReleasePopupHold();

// Shows the popup's flyout over the taskbar, in its own window: the taskbar's XAML root
// is only as tall as the taskbar. It opens on the chosen side, `gap` from the button.
void Present(Popup& p, const wux::FrameworkElement& anchor, bool taskbarOnTop) {
    p.anchor = winrt::make_weak(anchor);
    p.taskbarOnTop = taskbarOnTop;
    p.taskbar = g_folderTaskbar;
    // Out of context: called on this thread, which unhooks it as the popup closes.
    if (HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                                             OnForeground, 0, 0, WINEVENT_OUTOFCONTEXT))
        p.foreground = std::shared_ptr<void>(hook, [](HWINEVENTHOOK h) { UnhookWinEvent(h); });
    p.flyout.ShouldConstrainToRootBounds(false);
    // Standard, not Transient: the pointer that opened it would close it at once.
    p.flyout.ShowMode(wux::Controls::Primitives::FlyoutShowMode::Standard);
    Keep(p, p.flyout.Opened(winrt::auto_revoke, [](const auto&, const auto&) {
        Wh_Log(L"popup opened");
        AllowHostBackdrop();
        if (g_popup) StartWatch(*g_popup);
        if (g_popup && g_popup->name) {
            g_popup->name.Focus(wux::FocusState::Programmatic);
            g_popup->name.SelectAll();
        }
    }));
    // At the press that closes it, before that press's click could come.
    Keep(p, p.flyout.Closing(winrt::auto_revoke, [](const auto&, const auto&) {
        if (!g_popup) return;
        NoteMenuClosed(g_popup->model.folder);
        SwitchFolderOnPress(*g_popup);
    }));
    Keep(p, p.flyout.Closed(winrt::auto_revoke, [](const auto&, const auto&) {
        Wh_Log(L"popup closed");
        if (!g_popup) return;
        g_popup->foreground.reset();
        if (g_popup->watch) g_popup->watch.Stop();
        NoteMenuClosed(g_popup->model.folder);  // its name now, after a rename
        EndCellDrag(*g_popup, true);
        if (!g_popup->selfClose) GiveBackLater();
        ReleasePopupHold();
    }));
    bool below = p.look.side == Look::Side::Auto ? taskbarOnTop : p.look.side == Look::Side::Below;
    // The bubble sits kShadowRoom, at the folder size, inside the flyout's window.
    float offset = float(p.look.gap - kShadowRoom * UiScale(p.look));
    wux::Controls::Primitives::FlyoutShowOptions options;
    options.Placement(below ? wux::Controls::Primitives::FlyoutPlacementMode::Bottom
                            : wux::Controls::Primitives::FlyoutPlacementMode::Top);
    options.Position(wf::IReference<wf::Point>(wf::Point{
        float(anchor.ActualWidth() / 2), below ? float(anchor.ActualHeight()) + offset : -offset}));
    p.flyout.ShowAt(anchor, options);
    Wh_Log(L"popup shows, open: %d", p.flyout.IsOpen());
}

Popup& NewPopup(const std::wstring& folder, const wux::FrameworkElement& anchor) {
    ClosePopup();
    Popup& p = g_popup.emplace();
    p.model.folder = folder;
    p.look = CurrentLook();
    p.colors = anchor.ActualTheme() == wux::ElementTheme::Light ? &kLightPalette : &kDarkPalette;
    return p;
}

// `bubble` at the folder size. XAML has no layout scale, so it is drawn scaled inside a
// box kept at the scaled size, which the flyout's window takes.
wux::FrameworkElement Scaled(Popup& p, const wux::FrameworkElement& bubble) {
    double scale = UiScale(p.look);
    if (scale == 1) return bubble;
    wux::Media::ScaleTransform transform;
    transform.ScaleX(scale);
    transform.ScaleY(scale);
    bubble.RenderTransform(transform);
    wux::Controls::Canvas box;  // lays the bubble out at its own size
    box.Children().Append(bubble);
    auto fit = [box, scale](wf::Size size) {
        box.Width(size.Width * scale);
        box.Height(size.Height * scale);
    };
    constexpr float kAny = std::numeric_limits<float>::infinity();
    bubble.Measure({kAny, kAny});
    fit(bubble.DesiredSize());
    Keep(p, bubble.SizeChanged(winrt::auto_revoke, [fit](const auto&, const wux::SizeChangedEventArgs& args) {
        fit(args.NewSize());
    }));
    return box;
}

// Shows `body` in the popup's glass bubble.
void PresentBubble(Popup& p, const wux::FrameworkElement& body, double padding,
                   const wux::FrameworkElement& anchor, bool taskbarOnTop) {
    wux::Controls::Flyout flyout;
    flyout.FlyoutPresenterStyle(BarePresenter());
    flyout.Content(Scaled(p, BuildBubble(p, body, padding)));
    p.flyout = flyout;
    Present(p, anchor, taskbarOnTop);
}

void ShowRenamePopup(const std::wstring& folder, const wux::FrameworkElement& anchor,
                     bool taskbarOnTop) {
    Popup& p = NewPopup(folder, anchor);
    PresentBubble(p, BuildNameField(p), 10, anchor, taskbarOnTop);
}

constexpr double kMenuIconSize = 16;

// A menu row's icon: the picture, or the glyph when there is none.
wux::UIElement MenuIcon(const Popup& p, const wchar_t* glyph, const Pixels& pixels = {},
                        int size = 0) {
    if (auto bitmap = ToBitmap(pixels, size)) {
        wux::Controls::Image image;
        image.Width(kMenuIconSize);
        image.Height(kMenuIconSize);
        image.Source(bitmap);
        return image;
    }
    wux::Controls::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(kMenuIconSize);
    icon.Foreground(Brush(p.colors->text));
    return icon;
}

// A `width` wide menu row: `icon`, then `text`. A tap closes the menu, then runs `command`
// with the popup's folder (its name now, after a rename).
void AddMenuRow(Popup& p, const wux::Controls::StackPanel& rows, double width,
                const std::wstring& text, const wux::UIElement& icon,
                std::function<void(const std::wstring&)> command) {
    constexpr double kRowHeight = 38, kInset = 12;
    wux::Controls::StackPanel line;
    line.Orientation(wux::Controls::Orientation::Horizontal);
    line.VerticalAlignment(wux::VerticalAlignment::Center);
    line.Margin({kInset, 0, kInset, 0});
    wux::Controls::TextBlock label;
    label.Text(text);
    label.FontSize(14);
    label.Margin({kInset, 0, 0, 0});
    label.MaxWidth(width - 3 * kInset - kMenuIconSize);
    label.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    label.Foreground(Brush(p.colors->text));
    line.Children().Append(icon);
    line.Children().Append(label);
    auto row = Plate(p, width, kRowHeight, kMenuItemRadius, line);
    wux::Automation::AutomationProperties::SetName(row, text);
    Keep(p, row.Tapped(winrt::auto_revoke, [command](const auto&, const auto&) {
        if (!g_popup) return;
        std::wstring folder = g_popup->model.folder;
        HideSelf(*g_popup);
        command(folder);
    }));
    rows.Children().Append(row);
}

void AddMenuSeparator(const Popup& p, const wux::Controls::StackPanel& rows) {
    wux::Controls::Border separator;
    separator.Height(1);
    separator.Margin({10, 4, 10, 4});
    separator.Background(Brush(p.colors->border));
    rows.Children().Append(separator);
}

// Asks before the folder is dissolved, in the menu's bubble: the question, what happens,
// then the two answers.
void ShowDissolvePrompt(const std::wstring& folder, const wux::FrameworkElement& anchor,
                        bool taskbarOnTop) {
    constexpr double kRowWidth = 260;
    Popup& p = NewPopup(folder, anchor);
    wux::Controls::StackPanel rows;
    auto text = [&](const std::wstring& value, double size, const auto& color, double top) {
        wux::Controls::TextBlock block;
        block.Text(value);
        block.FontSize(size);
        block.MaxWidth(kRowWidth - 24);
        block.TextWrapping(wux::TextWrapping::Wrap);
        block.Margin({12, top, 12, 0});
        block.Foreground(Brush(color));
        rows.Children().Append(block);
    };
    text(L"정말 해제하시겠습니까?", 14, p.colors->text, 8);
    text(L"'" + FolderName(folder) + L"' 안의 앱은 작업 표시줄로 돌아가고, 폴더는 휴지통으로 갑니다.",
         12, p.colors->muted, 4);
    AddMenuSeparator(p, rows);
    AddMenuRow(p, rows, kRowWidth, L"해제하기", MenuIcon(p, L"\xE8B7"), [](const std::wstring& folder) {
        StartJob([folder] { DissolveFolder(folder); });
    });
    AddMenuRow(p, rows, kRowWidth, L"취소", MenuIcon(p, L"\xE711"), [](const std::wstring&) {});
    PresentBubble(p, rows, 6, anchor, taskbarOnTop);
}

// The folder's right-click menu, in place of the taskbar's jump list: rows in the same
// glass bubble as the folder's apps.
void ShowFolderMenu(const std::wstring& folder, const wux::FrameworkElement& anchor,
                    bool taskbarOnTop) {
    constexpr double kRowWidth = 212;
    Popup& p = NewPopup(folder, anchor);
    wux::Controls::StackPanel rows;
    auto weak = winrt::make_weak(anchor);
    auto add = [&](const wchar_t* text, const wchar_t* glyph, auto command) {
        AddMenuRow(p, rows, kRowWidth, text, MenuIcon(p, glyph), command);
    };
    if (IsLoose(folder)) {  // its apps come and go by themselves, so it only closes them
        add(L"모든 창 닫기", L"\xE711",
            [](const std::wstring&) { CloseWindows(LooseWindows(L"")); });
        return PresentBubble(p, rows, 6, anchor, taskbarOnTop);
    }
    // A row that swaps the menu for another popup on the same button, once its tap returns.
    auto then = [weak, taskbarOnTop](auto show) {
        return [weak, taskbarOnTop, show](const std::wstring& folder) {
            RunOnTaskbar([folder, weak, taskbarOnTop, show] {
                if (auto anchor = weak.get()) show(folder, anchor, taskbarOnTop);
            });
        };
    };
    add(L"이름 바꾸기", L"\xE8AC", then(ShowRenamePopup));
    add(L"아이콘 바꾸기", L"\xE8B9", [](const std::wstring& folder) {
        StartJob([folder] { WithOwnerWindow([&](HWND owner) { PickFolderIcon(owner, folder); }); });
    });
    AddMenuSeparator(p, rows);
    add(L"폴더 해제하기", L"\xE8B7", then(ShowDissolvePrompt));
    add(L"작업 표시줄에서 고정 해제", L"\xE77A", [](const std::wstring& folder) {
        StartJob([folder] { UnpinFolder(folder); });
    });
    PresentBubble(p, rows, 6, anchor, taskbarOnTop);
}

// An app's right-click menu in a folder, laid out as the taskbar's jump list: its recent
// documents, then the app itself (a new window), taking it out, closing its windows.
void ShowAppMenu(const AppMenuModel& model, const wux::FrameworkElement& anchor,
                 bool taskbarOnTop) {
    constexpr double kRowWidth = 280;
    Popup& p = NewPopup(model.folder, anchor);
    wux::Controls::StackPanel rows;
    if (!model.documents.empty()) {
        wux::Controls::TextBlock header;
        header.Text(L"최근 항목");
        header.FontSize(12);
        header.Margin({12, 6, 12, 4});
        header.Foreground(Brush(p.colors->muted));
        rows.Children().Append(header);
        for (const Document& document : model.documents)
            AddMenuRow(p, rows, kRowWidth, document.label,
                       MenuIcon(p, L"\xE8A5", document.icon, document.iconSize),
                       [document](const std::wstring&) {
                           StartJob([document] { OpenDocument(document); });
                       });
        AddMenuSeparator(p, rows);
    }
    std::wstring path = model.app.path;
    LooseWindow app = model.app.loose;
    std::function<void(const std::wstring&)> open = [path](const std::wstring&) {
        StartJob([path] { Open(nullptr, path); });
    };
    if (IsLoose(model.folder))
        open = [app](const std::wstring&) { StartJob([app] { OpenLooseApp(app); }); };
    AddMenuRow(p, rows, kRowWidth, model.app.label,
               MenuIcon(p, L"\xE7AC", model.app.icon, model.app.iconSize), open);
    if (model.shortcut)
        AddMenuRow(p, rows, kRowWidth, L"폴더에서 빼기", MenuIcon(p, L"\xE738"),
                   [path](const std::wstring& folder) {
                       StartJob([folder, path] { TakeOut(folder, path); });
                   });
    if (IsLoose(model.folder))
        AddMenuRow(p, rows, kRowWidth, L"작업 표시줄에 고정", MenuIcon(p, L"\xE718"),
                   [app](const std::wstring&) { StartJob([app] { PinLoose(app); }); });
    if (!model.windows.empty())
        AddMenuRow(p, rows, kRowWidth, model.windows.size() > 1 ? L"모든 창 닫기" : L"창 닫기",
                   MenuIcon(p, L"\xE711"),
                   [windows = model.windows](const std::wstring&) { CloseWindows(windows); });
    PresentBubble(p, rows, 6, anchor, taskbarOnTop);
}

// Right-click on an app in a folder: its menu takes the folder's place.
void OpenAppMenu(Popup& p, const wux::Controls::Grid& cell) {
    auto it = std::find(p.cells.begin(), p.cells.end(), cell);
    if (it == p.cells.end()) return;
    double scale = (cell.XamlRoot() ? cell.XamlRoot().RasterizationScale() : 1) * UiScale(p.look);
    int iconSize = int(kMenuIconSize * scale + 0.5);  // the pixels it is drawn at
    std::wstring folder = p.model.folder;
    PopupItem app = p.model.items[it - p.cells.begin()];
    auto anchor = p.anchor;
    bool taskbarOnTop = p.taskbarOnTop;
    HideSelf(p);
    StartJob([folder, app, iconSize, anchor, taskbarOnTop] {
        AppMenuModel model = LoadAppMenu(folder, app, iconSize);
        RunOnTaskbar([model, anchor, taskbarOnTop] {
            try {
                if (auto button = anchor.get()) ShowAppMenu(model, button, taskbarOnTop);
            } catch (...) {
                Wh_Log(L"app menu failed: %08X", winrt::to_hresult());
                ClosePopup();
            }
        });
    });
}

// The width of the anchor's screen in DIPs.
double WorkWidth(const wux::FrameworkElement& anchor) {
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &monitor);
    double scale = anchor.XamlRoot() ? anchor.XamlRoot().RasterizationScale() : 1;
    return (monitor.rcWork.right - monitor.rcWork.left) / scale;
}

void ShowFolderPopup(PopupModel model, const wux::FrameworkElement& anchor, bool taskbarOnTop) {
    Popup& p = NewPopup(model.folder, anchor);
    p.model = std::move(model);

    p.canvas = wux::Controls::Canvas();
    for (size_t i = 0; i < p.model.items.size(); i++) {
        p.cells.push_back(BuildCell(p, i));
        p.canvas.Children().Append(p.cells.back());
    }
    LayoutCells(p);
    // Never wraps: a row wider than the screen scrolls sideways.
    wux::Controls::ScrollViewer scroll;
    scroll.Content(p.canvas);
    scroll.HorizontalScrollMode(wux::Controls::ScrollMode::Enabled);
    scroll.HorizontalScrollBarVisibility(wux::Controls::ScrollBarVisibility::Auto);
    scroll.VerticalScrollMode(wux::Controls::ScrollMode::Disabled);
    scroll.VerticalScrollBarVisibility(wux::Controls::ScrollBarVisibility::Disabled);
    scroll.MaxWidth(WorkWidth(anchor) / UiScale(p.look) - 2 * kShadowRoom - 48);
    PresentBubble(p, scroll, 6, anchor, taskbarOnTop);
}

// The folder whose pin carries `appId`.
std::wstring FolderFromAppId(const std::wstring& appId) {
    std::wstring root = Root();
    for (const Entry& entry : root.empty() ? std::vector<Entry>{} : ListEntries(root))
        if (entry.isDir && _wcsicmp(FolderAppId(root + L"\\" + entry.name).c_str(), appId.c_str()) == 0)
            return root + L"\\" + entry.name;
    return {};
}

bool TaskbarOnTop(POINT pt) {
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &monitor);
    return pt.y < monitor.rcWork.top;
}

// Found by symbol only, never hooked.
using TryGetGroupViewModel_t = void*(WINAPI*)(void** result, void* const* element);
TryGetGroupViewModel_t TryGetGroupViewModel;
using HoverFlyoutDismiss_t = void(WINAPI*)(void* model, bool force);
HoverFlyoutDismiss_t HoverFlyoutDismiss;
using HoverFlyoutVisible_t = bool(WINAPI*)(const void* model);
HoverFlyoutVisible_t HoverFlyoutVisible;

// The previews live in a window of their own that fades out before it hides, and hiding
// it takes the activation along: a popup opened during the fade closes at once. So a
// click that closes previews opens its folder once they are gone, or after a while.
// Taskbar thread only.
constexpr ULONGLONG kPreviewCloseWaitMs = 400;  // the fade is 167 ms
struct PreviewWait {
    void* model = nullptr;  // the hover model the click closed
    ULONGLONG since = 0;
    bool gone = false;
    HWND taskbar = nullptr;
    std::function<void()> show;  // held until then
    wux::DispatcherTimer timer{nullptr};
};
[[clang::no_destroy]] std::optional<PreviewWait> g_previewWait;

void DropPreviewWait() {
    if (g_previewWait && g_previewWait->timer) g_previewWait->timer.Stop();
    g_previewWait.reset();
}

std::wstring ClassOf(HWND window) {
    wchar_t name[64] = L"";
    if (window) GetClassNameW(window, name, ARRAYSIZE(name));
    return name;
}

// The taskbar window under `pt`.
HWND TaskbarAt(POINT pt) {
    HWND root = GetAncestor(WindowFromPoint(pt), GA_ROOT);
    std::wstring name = ClassOf(root);
    return name == L"Shell_TrayWnd" || name == L"Shell_SecondaryTrayWnd" ? root : nullptr;
}

// A button under previews takes no activation when pressed; the folder's popup needs the
// taskbar active, as any other click leaves it.
void ActivateTaskbar(HWND taskbar) {
    if (!taskbar || GetForegroundWindow() == taskbar) return;
    BOOL ok = SetForegroundWindow(taskbar);
    Wh_Log(L"taskbar to the front: %d", ok);  // Wh_Log skips its arguments when logging is off
}

// That fails once the pointer has gone on to an app (explorer no longer has the last input)
// after the previews' hiding took the activation: the popup then shows over a taskbar
// Windows does not see active, which gets no light dismiss and no foreground change, and
// whose tray's hide check takes it in from under the popup. So from the click that closed
// previews until the popup closes, the check skips its ticks on a taskbar that is not
// active, as it keeps an active one out; the taskbar is asked to the front again as the
// pointer comes onto a window of this thread, the input explorer needs; a press elsewhere
// closes the popup, as the light dismiss would; and a taskbar that goes in all the same
// takes the popup along. Taskbar thread only.
constexpr ULONGLONG kComingHoldMs = 2000;  // a folder whose popup never comes lets go
constexpr int kWatchMs = 40;               // shorter than a click
HWND g_popupHeld = nullptr;  // the taskbar whose check skips ticks for a popup

// core::g_popupDropsTimer.
bool PopupDropsTimer(HWND taskbar, UINT_PTR id) {
    if (id != autohide::kTrayHideTimer || g_unloading) return false;
    bool shows = g_popup && g_popup->taskbar == taskbar && g_popup->flyout && g_popup->flyout.IsOpen();
    bool coming = g_previewWait && g_previewWait->taskbar == taskbar &&
                  GetTickCount64() - g_previewWait->since < kComingHoldMs;
    HWND front = GetForegroundWindow();
    if ((!shows && !coming) || front == taskbar) {
        if (g_popupHeld == taskbar) g_popupHeld = nullptr;  // this tick goes on to the check
        return false;
    }
    if (std::exchange(g_popupHeld, taskbar) != taskbar)
        core::DiagLine(L"Popup holds %p: foreground %s", taskbar, ClassOf(front).c_str());
    return true;
}

// The check runs on from where it is; the primary's is started again, as after the
// launcher's hold, in case something stopped it meanwhile.
void ReleasePopupHold() {
    HWND taskbar = std::exchange(g_popupHeld, nullptr);
    if (!taskbar || g_unloading) return;
    core::DiagLine(L"Popup lets go %p", taskbar);
    RunOnTaskbar([taskbar] {  // outside the closing's calls
        if (IsWindow(taskbar) && !core::IsSecondary(taskbar) && !autohide::LooksHidden(taskbar))
            autohide::ResumeAutoHide(taskbar);
    });
}

void WatchTick() {
    try {
        if (!g_popup || g_unloading) return;
        Popup& p = *g_popup;
        if (!p.flyout || !p.flyout.IsOpen()) {
            if (p.watch) p.watch.Stop();
            return;
        }
        HWND front = GetForegroundWindow();
        if (front == p.taskbar) return;
        // Held now, or pressed since the last look: a tap can be shorter than kWatchMs.
        // Read only here, so an ordinary popup leaves that mark to other programs.
        bool pressed = (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) |
                        GetAsyncKeyState(VK_MBUTTON)) & 0x8001;
        if (p.taskbar && autohide::LooksHidden(p.taskbar)) {
            core::DiagLine(L"Popup closes: taskbar %p went in", p.taskbar);
            return HideSelf(p);
        }
        // Another window of this thread in front: a press elsewhere moves the foreground.
        POINT pt;
        if (OnThisThread(front) || p.down || p.dragging || !GetCursorPos(&pt)) return;
        if (!OnThisThread(WindowFromPoint(pt))) {
            p.activating = false;
            if (!pressed) return;
            core::DiagLine(L"Popup closes: press outside");
            return HideSelf(p);
        }
        // A press here surely gives explorer the input, where a move may not: asked again after it.
        if (pressed) p.activating = false;
        else if (!std::exchange(p.activating, true)) ActivateTaskbar(p.taskbar);
    } catch (...) {
    }
}

// Once the flyout is open (Opened); WatchTick stops it should the flyout close unseen.
void StartWatch(Popup& p) {
    if (p.watch) p.watch.Stop();
    for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) GetAsyncKeyState(button);  // the opening click's mark
    p.watch = wux::DispatcherTimer();
    p.watch.Interval(std::chrono::milliseconds(kWatchMs));
    p.watch.Tick([](const wf::IInspectable&, const wf::IInspectable&) { WatchTick(); });
    p.watch.Start();
}

// A popup dismissed with nothing picked (Esc, a press on the taskbar's empty part or on its
// own button) leaves the taskbar the active window, and Windows keeps an active taskbar out
// with auto-hide on until something else is clicked. So the app window in front gets the
// activation back, once no button is held and nothing else took it: a press that goes on to
// open a window or another folder comes first. Taskbar thread only.
constexpr int kGiveBackMs = 300;
constexpr ULONGLONG kGiveBackHeldMs = 3000;  // a press held this long is left alone
struct GiveBack {
    ULONGLONG since = 0;
    wux::DispatcherTimer timer{nullptr};
};
[[clang::no_destroy]] std::optional<GiveBack> g_giveBack;

void DropGiveBack() {
    if (g_giveBack && g_giveBack->timer) g_giveBack->timer.Stop();
    g_giveBack.reset();
}

void GiveBackTick() {
    if (!g_giveBack) return;
    bool held = (GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON)) & 0x8000;
    bool popup = (g_popup && g_popup->flyout && g_popup->flyout.IsOpen()) || g_previewWait;
    if (held && !popup && GetTickCount64() - g_giveBack->since < kGiveBackHeldMs) return;  // next tick
    DropGiveBack();
    std::wstring front = ClassOf(GetForegroundWindow());
    if (held || popup || g_unloading || (front != L"Shell_TrayWnd" && front != L"Shell_SecondaryTrayWnd"))
        return;
    HWND to = nullptr;
    for (HWND window : ButtonWindows())
        if (!(GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOPMOST) && !IsIconic(window)) {
            to = window;
            break;
        }
    if (!to) to = GetShellWindow();  // no app window: the desktop, as after the last one closes
    BOOL ok = to && SetForegroundWindow(to);
    Wh_Log(L"activation back from the taskbar: %d", ok);
}

void GiveBackLater() {
    DropGiveBack();
    GiveBack& g = g_giveBack.emplace();
    g.since = GetTickCount64();
    g.timer = wux::DispatcherTimer();
    g.timer.Interval(std::chrono::milliseconds(kGiveBackMs));
    g.timer.Tick([](const wf::IInspectable&, const wf::IInspectable&) { GiveBackTick(); });
    g.timer.Start();
}

void ShowHeldPopup(const wchar_t* after) {
    if (!g_previewWait) return;
    std::function<void()> show = std::move(g_previewWait->show);
    HWND taskbar = g_previewWait->taskbar;
    ULONGLONG waited = GetTickCount64() - g_previewWait->since;
    DropPreviewWait();
    if (!show) return;
    Wh_Log(L"popup after %s, %llu ms", after, waited);
    if (taskbar && autohide::LooksHidden(taskbar)) {  // gone in meanwhile: no popup over nothing
        core::DiagLine(L"Popup left: taskbar %p went in", taskbar);
        return ReleasePopupHold();
    }
    ActivateTaskbar(taskbar);
    show();
}

// Opens a folder's popup now, or once the previews its click closed are gone.
void ShowAfterPreviews(std::function<void()> show) {
    if (!g_previewWait || g_previewWait->gone ||
        GetTickCount64() - g_previewWait->since >= kPreviewCloseWaitMs) {
        if (g_previewWait) {
            g_previewWait->show = std::move(show);
            return ShowHeldPopup(g_previewWait->gone ? L"previews closed" : L"timeout");
        }
        return show();
    }
    PreviewWait& wait = *g_previewWait;
    wait.show = std::move(show);
    if (wait.timer) return;  // a second click while waiting takes the first one's place
    wait.timer = wux::DispatcherTimer();
    wait.timer.Interval(std::chrono::milliseconds(kPreviewCloseWaitMs - (GetTickCount64() - wait.since)));
    wait.timer.Tick([](const wf::IInspectable&, const wf::IInspectable&) { ShowHeldPopup(L"timeout"); });
    wait.timer.Start();
}

// The previews' window has just hidden (CommitDismissFlyout ends every way they close).
// Its controller's hover model (+0xD8) tells which taskbar's previews they were.
using CommitDismissFlyout_t = void(WINAPI*)(void* pThis);
CommitDismissFlyout_t CommitDismissFlyout_Original;
void WINAPI CommitDismissFlyout_Hook(void* pThis) {
    HWND before = GetForegroundWindow();
    CommitDismissFlyout_Original(pThis);
    if (g_unloading) return;
    void* model = *reinterpret_cast<void**>(static_cast<BYTE*>(pThis) + 0xD8);
    HWND after = GetForegroundWindow();
    // After the activation and focus moves the hiding set off.
    RunOnTaskbar(
        [model, before, after] {
            if (!g_previewWait || g_previewWait->model != model) return;
            Wh_Log(L"previews gone, foreground %s -> %s", ClassOf(before).c_str(), ClassOf(after).c_str());
            g_previewWait->gone = true;
            if (g_previewWait->show) ShowHeldPopup(L"previews closed");
        },
        false, winrt::Windows::System::DispatcherQueuePriority::Low);
}

// The hover model at `field`, if what is there looks like one: readable, with its vtable
// in the module its functions come from. Else null, and the previews stay.
void* HoverModelAt(void* const* field) {
    if (!autohide::Readable(field, sizeof(void*))) return nullptr;
    void* model = *field;
    if (!autohide::Readable(model, sizeof(void*))) return nullptr;
    constexpr DWORD kFlags =
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    HMODULE expected = nullptr, found = nullptr;
    if (!GetModuleHandleExW(kFlags, reinterpret_cast<LPCWSTR>(HoverFlyoutDismiss), &expected) ||
        !GetModuleHandleExW(kFlags, static_cast<LPCWSTR>(*static_cast<void**>(model)), &found) ||
        found != expected) {
        Wh_Log(L"previews stay: no hover model at %p", field);
        return nullptr;
    }
    return model;
}

// The window previews over a button close once the taskbar acts on the click, which a
// folder skips; so they close the way a right-click closes them, through the hover model
// the button's view model holds (+0xA0 of the object behind its interface at +8). True if
// they were showing; the folder then waits for them (ShowAfterPreviews).
bool DismissHoverFlyout(const wux::FrameworkElement& button, POINT pt) {
    if (!TryGetGroupViewModel || !HoverFlyoutDismiss) return false;
    try {
        wux::UIElement element = button;
        wf::IInspectable viewModel{nullptr};
        TryGetGroupViewModel(winrt::put_abi(viewModel), reinterpret_cast<void* const*>(&element));
        if (!viewModel) return false;
        if (auto name = winrt::get_class_name(viewModel); name != L"Taskbar.TaskListGroupViewModel") {
            Wh_Log(L"previews stay: view model is %s", name.c_str());
            return false;
        }
        auto object = static_cast<BYTE*>(winrt::get_abi(viewModel)) - 8;
        void* model = HoverModelAt(reinterpret_cast<void* const*>(object + 0xA0));
        if (!model) return false;
        bool showing = HoverFlyoutVisible && HoverFlyoutVisible(model);
        if (showing) {
            // Before the dismissal: with animations off, the previews hide inside it.
            DropPreviewWait();
            g_previewWait.emplace();
            g_previewWait->model = model;
            g_previewWait->since = GetTickCount64();
            g_previewWait->taskbar = TaskbarAt(pt);
        }
        HoverFlyoutDismiss(model, true);
        if (showing) ActivateTaskbar(g_previewWait ? g_previewWait->taskbar : nullptr);
        return showing;
    } catch (...) {
        Wh_Log(L"previews stay: %08X", winrt::to_hresult());
        return false;
    }
}

// The folder a press switched to, until its popup closes, so that the press's click, should
// it come, opens nothing more. Taskbar thread.
[[clang::no_destroy]] std::wstring g_switchedTo;  // app ID
ULONGLONG g_switchTick = 0;
constexpr ULONGLONG kSwitchClickMs = 3000;  // a click held longer is a new one

// Opens the popup of the folder whose button is `button` (app ID `id`), clicked at `pt`;
// the menu takes over if the popup cannot be shown.
void OpenFolderOf(const wux::FrameworkElement& button, const std::wstring& id, POINT pt) {
    g_folderTaskbar = TaskbarAt(pt);
    if (g_previewWait && GetTickCount64() - g_previewWait->since >= kPreviewCloseWaitMs)
        DropPreviewWait();  // left by a click whose folder never came
    std::wstring foreground = ClassOf(GetForegroundWindow());
    bool previews = DismissHoverFlyout(button, pt);
    Wh_Log(L"folder click, previews were showing: %d, foreground %s", previews, foreground.c_str());
    auto root = button.XamlRoot();
    Look look = CurrentLook();  // icons read at the pixels they are drawn at
    int iconSize = int(look.iconSize * UiScale(look) * (root ? root.RasterizationScale() : 1) + 0.5);
    StartJob([id, iconSize, pt, weak = winrt::make_weak(button)] {
        std::wstring folder = IsLooseId(id) ? std::wstring(kLooseFolder) : FolderFromAppId(id);
        if (folder.empty() || JustClosed(folder)) {
            Wh_Log(L"folder click left alone: %s", folder.empty() ? L"no folder" : L"just closed");
            return;
        }
        auto show = [model = LoadPopupModel(folder, iconSize), weak, pt, folder] {
            try {
                if (auto anchor = weak.get()) return ShowFolderPopup(model, anchor, TaskbarOnTop(pt));
            } catch (...) {
                Wh_Log(L"popup failed: %08X", winrt::to_hresult());
                ClosePopup();
            }
            g_switchedTo.clear();  // the menu's closing would not clear it
            StartJob([folder, pt] { RunFolderMenu(folder, pt); });
        };
        if (!RunOnTaskbar([show] { ShowAfterPreviews(show); })) RunFolderMenu(folder, pt);
    });
}

// The taskbar button under the screen point `pt` on `taskbar`, the window whose XAML
// island (filling it) holds `anchor`.
wux::FrameworkElement TaskbarButtonAt(const wux::FrameworkElement& anchor, HWND taskbar, POINT pt) {
    HWND island = FindWindowExW(taskbar, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    auto root = anchor.XamlRoot();
    RECT rect;
    if (!island || !root || !root.Content() || !GetWindowRect(island, &rect)) return nullptr;
    double scale = root.RasterizationScale();
    wf::Point at{float((pt.x - rect.left) / scale), float((pt.y - rect.top) / scale)};
    for (const wux::UIElement& element : wux::Media::VisualTreeHelper::FindElementsInHostCoordinates(at, root.Content()))
        if (auto button = element.try_as<wux::FrameworkElement>();
            button && winrt::get_class_name(button) == kTaskListButtonClass)
            return button;
    return nullptr;
}

// The press that closes a popup goes to its light dismiss, never to the taskbar under it;
// on another folder's button it opens that folder, as a click there would. Runs as any
// popup starts closing.
void SwitchFolderOnPress(const Popup& p) {
    g_switchedTo.clear();
    if (p.selfClose || p.down || p.dragging) return;
    // As the press left the thread's input state, or else still held.
    bool pressed = (GetKeyState(VK_LBUTTON) & 0x8000) ||
                   (GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000);
    if (!pressed) return;
    POINT pt;
    GetCursorPos(&pt);
    auto anchor = p.anchor.get();
    if (!anchor || !p.taskbar || TaskbarAt(pt) != p.taskbar) return;
    auto button = TaskbarButtonAt(anchor, p.taskbar, pt);
    std::wstring id = button && button != anchor ? ButtonAppId(button, false) : L"";
    if (!id.starts_with(kAppIdPrefix)) return;
    Wh_Log(L"press on another folder: %s", id.c_str());
    g_switchedTo = id;
    g_switchTick = GetTickCount64();
    RunOnTaskbar([weak = winrt::make_weak(button), id, pt] {  // once this popup has closed
        if (auto button = weak.get()) OpenFolderOf(button, id, pt);
    });
}

using Invoke_t = bool(WINAPI*)(void* pThis, void* pointerPoint, UINT64 key);
Invoke_t Invoke_Original;
// A click on a folder button opens its popup instead of launching the pin.
bool WINAPI Invoke_Hook(void* pThis, void* pointerPoint, UINT64 key) {
    if (!g_unloading) {
        try {
            NoteTaskbarThread();
            auto button = ButtonFromThis(pThis);
            std::wstring id = button ? ButtonAppId(button, false) : L"";
            if (id.starts_with(kAppIdPrefix)) {
                bool switched = id == g_switchedTo && GetTickCount64() - g_switchTick < kSwitchClickMs;
                g_switchedTo.clear();
                if (switched) {
                    Wh_Log(L"folder click left alone: opened at its press");
                    return true;
                }
                POINT pt;
                GetCursorPos(&pt);
                OpenFolderOf(button, id, pt);
                return true;
            }
        } catch (...) {
        }
    }
    return Invoke_Original(pThis, pointerPoint, key);
}

// A folder's right-click opens its menu instead of the taskbar's jump list; a folder
// that is gone from disk keeps the jump list, with its unpin.
bool OpenFolderMenu(const wux::UIElement& sender, void* args) {
    if (g_unloading) return false;
    try {
        NoteTaskbarThread();
        auto button = sender.try_as<wux::FrameworkElement>();
        std::wstring id = button ? ButtonAppId(button, false) : L"";
        std::wstring folder = IsLooseId(id)                ? std::wstring(kLooseFolder)
                              : id.starts_with(kAppIdPrefix) ? FolderFromAppId(id)
                                                             : L"";
        if (folder.empty()) return false;
        // Skipping the original is what keeps the jump list away; the event args are left
        // alone (marking them handled crashed explorer), and the menu opens once the
        // taskbar is done with the event, as the folder's own popup does.
        Wh_Log(L"menu for %s", folder.c_str());
        POINT pt;
        GetCursorPos(&pt);
        g_folderTaskbar = TaskbarAt(pt);
        RunOnTaskbar([folder, pt, weak = winrt::make_weak(button)] {
            try {
                if (auto anchor = weak.get()) ShowFolderMenu(folder, anchor, TaskbarOnTop(pt));
                Wh_Log(L"menu shown");
            } catch (...) {
                Wh_Log(L"menu failed: %08X", winrt::to_hresult());
                ClosePopup();
            }
        });
        return true;
    } catch (...) {
        Wh_Log(L"menu failed: %08X", winrt::to_hresult());
        ClosePopup();
        return false;
    }
}

using OnTaskListButtonContextRequested_t = void(WINAPI*)(void* pThis, const wux::UIElement& sender,
                                                         void* args);
OnTaskListButtonContextRequested_t OnTaskListButtonContextRequested_Original;
void WINAPI OnTaskListButtonContextRequested_Hook(void* pThis, const wux::UIElement& sender,
                                                  void* args) {
    if (!OpenFolderMenu(sender, args)) OnTaskListButtonContextRequested_Original(pThis, sender, args);
}

// The same, on builds that skip the one above.
using HandleContextRequested_t = void(WINAPI*)(const wux::UIElement& sender, void* args);
HandleContextRequested_t HandleContextRequested_Original;
void WINAPI HandleContextRequested_Hook(const wux::UIElement& sender, void* args) {
    if (!OpenFolderMenu(sender, args)) HandleContextRequested_Original(sender, args);
}

// The look of a folder button an app is held over: a tile grows under it and its icon
// settles into the tile.
void ShowTargetLook(const wux::FrameworkElement& target, bool on) {
    Touch(target);
    ShowPlate(target, on);
    Animate(IconPanel(target), on ? kTargetInPlate : 1, 1, kNormalMs, on);
}

// The target settles into a growing folder tile while the dragged icon shrinks over it.
void ShowMergeVisual(DragState& d, bool on) {
    auto target = d.target.get();
    auto dragged = d.dragged.get();
    if (!target || on == d.armed) return;
    d.armed = on;
    ShowTargetLook(target, on);
    if (dragged) {
        Touch(dragged);
        Animate(IconPanel(dragged), on ? kDraggedOverPlate : 1, on ? kDraggedOpacity : 1,
                kFastMs, on);
    }
}

// Once a merge job is done: everything eases back, and an existing folder that took the
// app bounces.
void SettleMerge(const std::wstring& bounceId) {
    ResetTouched(true);
    auto row = g_buttonRow.get();
    if (bounceId.empty() || !row) return;
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(row);
    for (int i = 0; i < count; i++)
        if (auto button = wux::Media::VisualTreeHelper::GetChild(row, i)
                              .try_as<wux::FrameworkElement>();
            button && winrt::get_class_name(button) == kTaskListButtonClass &&
            ButtonAppId(button, false) == bounceId)
            return Bounce(button);
}

void SetTarget(DragState& d, const wux::FrameworkElement& button, std::wstring id) {
    ShowMergeVisual(d, false);
    d.target = button ? winrt::make_weak(button) : nullptr;
    d.targetId = std::move(id);
    d.timer.Stop();
    if (button) d.timer.Start();
}

// Stops the timer, restores the target's look and forgets the drag.
void EndDrag() {
    if (DragState* d = CurrentDrag()) {
        try {
            SetTarget(*d, nullptr, L"");
        } catch (...) {
        }
    }
    g_drag.reset();
}

using OnDragStartedGesture_t = void(WINAPI*)(void* pThis, wf::Point point);
OnDragStartedGesture_t OnDragStartedGesture_Original;
void WINAPI OnDragStartedGesture_Hook(void* pThis, wf::Point point) {
    EndDrag();
    try {
        NoteTaskbarThread();
        if (!g_unloading) ResetTouched(false);  // leftovers of the last merge
        auto button = g_unloading ? nullptr : ButtonFromThis(pThis);
        std::wstring id = button ? ButtonAppId(button) : L"";
        Wh_Log(L"drag start: button=%d id=%s", bool(button), id.c_str());
        if (!id.empty() && !id.starts_with(kAppIdPrefix)) {  // folders don't go into folders
            DragState& d = g_drag.emplace();
            d.thread = GetCurrentThreadId();
            d.dragged = winrt::make_weak(button);
            d.draggedId = id;
            g_buttonRow = winrt::make_weak(wux::Media::VisualTreeHelper::GetParent(button));
            d.timer = wux::DispatcherTimer();
            d.timer.Interval(std::chrono::milliseconds(g_mergeDelayMs.load()));
            d.timer.Tick([](const wf::IInspectable& sender, const wf::IInspectable&) {
                if (auto timer = sender.try_as<wux::DispatcherTimer>()) timer.Stop();
                if (DragState* d = CurrentDrag(); d && !g_unloading && !d->targetId.empty()) {
                    Wh_Log(L"merge armed on %s", d->targetId.c_str());
                    ShowMergeVisual(*d, true);
                }
            });
        }
    } catch (...) {
        EndDrag();
    }
    OnDragStartedGesture_Original(pThis, point);
}

using UpdateDrag_t = void(WINAPI*)(void* pThis, wf::Point point);
UpdateDrag_t UpdateDrag_Original;
void WINAPI UpdateDrag_Hook(void* pThis, wf::Point point) {
    if (DragState* d = CurrentDrag(); d && !g_unloading) {
        try {
            if (auto dragged = d->dragged.get()) {
                if (!d->updates++) {
                    auto offset = dragged.ActualOffset();
                    auto parent = wux::Media::VisualTreeHelper::GetParent(dragged);
                    Wh_Log(L"first update: point %.0f,%.0f, dragged at %.0f,%.0f size %.0f, "
                           L"parent %s with %d children",
                           point.X, point.Y, offset.x, offset.y, dragged.ActualWidth(),
                           parent ? winrt::get_class_name(parent).c_str() : L"-",
                           parent ? wux::Media::VisualTreeHelper::GetChildrenCount(parent) : 0);
                }
                Hover hover = HoverAt(dragged, point);
                // Hold the drop placeholder still so the neighbour does not slide away.
                bool frozen = hover.zone != Zone::Free && d->hasLastBounds;
                if (frozen != d->frozen) Wh_Log(L"hold %d at %.0f", frozen, point.X);
                d->frozen = frozen;
                auto target = hover.zone == Zone::Merge ? hover.button : nullptr;
                std::wstring id = target ? ButtonAppId(target) : L"";
                if (IsLooseId(id)) id.clear();  // the loose folder takes no merges
                if (id != d->targetId) {
                    Wh_Log(L"target: '%s' at %.0f,%.0f", id.c_str(), point.X, point.Y);
                    SetTarget(*d, id.empty() ? nullptr : target, id);
                }
            }
        } catch (...) {
        }
    }
    UpdateDrag_Original(pThis, point);
}

using OnDragCompletedGesture_t = void(WINAPI*)(void* pThis);
OnDragCompletedGesture_t OnDragCompletedGesture_Original;
void WINAPI OnDragCompletedGesture_Hook(void* pThis) {
    DragState* d = CurrentDrag();
    bool merge = d && d->armed && !g_unloading;
    // The dragged icon goes into the folder instead of flying back to its slot.
    if (merge)
        if (auto dragged = d->dragged.get()) {
            try {
                Animate(IconPanel(dragged), 0.4f, 0, 120, true);
            } catch (...) {
            }
        }
    OnDragCompletedGesture_Original(pThis);
    if (!(d = CurrentDrag())) return;
    std::wstring dragged = d->draggedId, target = d->targetId;
    Wh_Log(L"drag end: merge=%d after %d updates", merge, d->updates);
    d->armed = false;  // the look stays until the merge job is done
    EndDrag();
    if (!merge) return ResetTouched(true);
    StartJob([dragged, target] {
        std::wstring bounce = MergeApps(dragged, target);
        RunOnTaskbar([bounce] { SettleMerge(bounce); });
    });
}

// Returns DropPlaceholder by value, hence the hidden result pointer.
using GetDropPlaceholder_t = void*(WINAPI*)(void* pThis, void* result, void* itemBounds,
                                            UINT64 itemRange, wf::Rect dragBounds,
                                            int direction, void* calculator, bool flag);
GetDropPlaceholder_t GetDropPlaceholder_Original;
void* WINAPI GetDropPlaceholder_Hook(void* pThis, void* result, void* itemBounds,
                                     UINT64 itemRange, wf::Rect dragBounds, int direction,
                                     void* calculator, bool flag) {
    if (DragState* d = CurrentDrag()) {
        if (d->frozen) {
            dragBounds = d->lastBounds;
        } else {
            d->lastBounds = dragBounds;
            d->hasLastBounds = true;
        }
    }
    return GetDropPlaceholder_Original(pThis, result, itemBounds, itemRange, dragBounds,
                                       direction, calculator, flag);
}

// True once for the button `id` names in the pending merge handoff's `field`.
bool TakeMergeMotion(std::wstring MergeMotion::*field, const std::wstring& id) {
    AcquireSRWLockExclusive(&g_lock);
    bool fresh = GetTickCount64() - g_mergeMotion.tick < 3000;
    bool match = fresh && !id.empty() && _wcsicmp((g_mergeMotion.*field).c_str(), id.c_str()) == 0;
    if (match) (g_mergeMotion.*field).clear();
    if (!fresh || (g_mergeMotion.enterId.empty() && g_mergeMotion.exitId.empty()))
        g_mergeMotionPending = false;
    ReleaseSRWLockExclusive(&g_lock);
    return match;
}

struct Float3 {  // winrt::Windows::Foundation::Numerics::float3, passed by value
    float x, y, z;
};
using PlayEntranceAnimation_t = void(WINAPI*)(void* pThis, int kind, double delay, Float3 offset,
                                              INT64 duration, bool flag);
PlayEntranceAnimation_t PlayEntranceAnimation_Original;
// A new folder button zooms in where its merge target was (kind 2 = zoom).
void WINAPI PlayEntranceAnimation_Hook(void* pThis, int kind, double delay, Float3 offset,
                                       INT64 duration, bool flag) {
    if ((g_mergeMotionPending || !g_touched.empty()) && !g_unloading) {
        try {
            if (auto button = ButtonFromThis(pThis)) {
                ResetButton(button, false);  // a recycled element may still carry our look
                if (TakeMergeMotion(&MergeMotion::enterId, ButtonAppId(button, false))) {
                    kind = 2;
                    delay = 0;
                }
            }
        } catch (...) {
        }
    }
    PlayEntranceAnimation_Original(pThis, kind, delay, offset, duration, flag);
}

using StartExitAnimation_t = void(WINAPI*)(void* pThis);
StartExitAnimation_t StartExitAnimation_Original;
// The merge target leaves without sliding away while the folder zooms in on its spot.
void WINAPI StartExitAnimation_Hook(void* pThis) {
    if (g_mergeMotionPending && !g_unloading) {
        try {
            if (auto button = ButtonFromThis(pThis);
                button && TakeMergeMotion(&MergeMotion::exitId, ButtonAppId(button, false)))
                return;
        } catch (...) {
        }
    }
    StartExitAnimation_Original(pThis);
}

// ---- Carry out of the popup (taskbar thread) ---------------------------------------

// A cell dragged past the popup's bubble goes on as its icon, drawn in a window of its own
// that follows the pointer over the taskbars and the desktop while the popup closes. Where
// it is let go decides: on another folder's button the app moves into that folder, on its
// own folder's button nothing happens, elsewhere on a taskbar it is pinned in the gap
// under the pointer, off the taskbars it is pinned next to its folder; Esc cancels.
// The window holds the mouse capture, so the taskbar under it shows no hover or previews,
// and reads the button and Esc on a timer too: the capture can go before the release (it
// is let go over a hidden taskbar's edge, so that the taskbar can come out).

constexpr wchar_t kIslandClass[] = L"Windows.UI.Composition.DesktopWindowContentBridge";

// Screen pixels to an element's DIPs and back, through the XAML island filling its taskbar.
struct BarSpace {
    RECT island{};
    double scale = 1;
    wf::Point origin{};  // the element's top left in its root's DIPs
};

std::optional<BarSpace> SpaceOf(HWND taskbar, const wux::UIElement& element) {
    HWND island = taskbar ? FindWindowExW(taskbar, nullptr, kIslandClass, nullptr) : nullptr;
    auto root = element.XamlRoot();
    BarSpace space;
    if (!island || !root || !root.Content() || !GetWindowRect(island, &space.island))
        return std::nullopt;
    space.scale = root.RasterizationScale();
    space.origin = element.TransformToVisual(root.Content()).TransformPoint({0, 0});
    return space;
}

wf::Point ToLocal(const BarSpace& space, POINT pt) {
    return {float((pt.x - space.island.left) / space.scale - space.origin.X),
            float((pt.y - space.island.top) / space.scale - space.origin.Y)};
}

POINT ToScreen(const BarSpace& space, wf::Point at) {
    return {LONG(std::lround(space.island.left + (at.X + space.origin.X) * space.scale)),
            LONG(std::lround(space.island.top + (at.Y + space.origin.Y) * space.scale))};
}

// The row (an ItemsRepeater) that holds a taskbar's buttons, found from one of them.
wux::FrameworkElement FindButtonRow(HWND taskbar) {
    auto root = core::TaskbarRoot(taskbar);
    auto content = root ? root.Content().try_as<wux::FrameworkElement>() : nullptr;
    auto button = core::FindDescendant(
        content, [](auto& e) { return winrt::get_class_name(e) == kTaskListButtonClass; }, 40);
    return button ? wux::Media::VisualTreeHelper::GetParent(button).try_as<wux::FrameworkElement>()
                  : nullptr;
}

// Each button in `row` that takes room, with its layout slot in the row.
template <typename F>
void ForEachButton(const wux::FrameworkElement& row, F&& visit) {
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(row);
    for (int i = 0; i < count; i++) {
        auto button = wux::Media::VisualTreeHelper::GetChild(row, i).try_as<wux::FrameworkElement>();
        if (!button || winrt::get_class_name(button) != kTaskListButtonClass ||
            button.Visibility() != wux::Visibility::Visible || button.ActualWidth() <= 0)
            continue;
        auto offset = button.ActualOffset();  // layout slot, unaffected by our scale
        visit(button, wf::Rect{offset.x, offset.y, float(button.ActualWidth()),
                               float(button.ActualHeight())});
    }
}

wux::FrameworkElement ButtonWithId(const wux::FrameworkElement& row, const std::wstring& id) {
    wux::FrameworkElement found{nullptr};
    ForEachButton(row, [&](const wux::FrameworkElement& button, wf::Rect) {
        if (!found && ButtonAppId(button, false) == id) found = button;
    });
    return found;
}

// What of a taskbar is at a screen point. `row` stays null off the buttons' height and on
// a taskbar that is not all on screen (auto-hidden, or on its way).
struct BarHit {
    HWND taskbar = nullptr;
    wux::FrameworkElement row{nullptr};
    BarSpace space;
    wf::Point at{};  // the point in the row
    wux::FrameworkElement button{nullptr};
    std::wstring id;      // the button's app ID
    bool middle = false;  // in its middle half, away from the gaps
};

BarHit HitRow(HWND taskbar, const wux::FrameworkElement& row, POINT pt) {
    BarHit hit{taskbar};
    auto space = row ? SpaceOf(taskbar, row) : std::nullopt;
    if (!space) return hit;
    float height = float(row.ActualHeight());
    POINT top = ToScreen(*space, {0, 0}), bottom = ToScreen(*space, {0, height});
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(core::MonitorOf(taskbar), &monitor);
    wf::Point at = ToLocal(*space, pt);
    if (top.y < monitor.rcMonitor.top - 1 || bottom.y > monitor.rcMonitor.bottom + 1 || at.Y < 0 ||
        at.Y > height)
        return hit;
    hit.row = row;
    hit.space = *space;
    hit.at = at;
    ForEachButton(row, [&](const wux::FrameworkElement& button, wf::Rect slot) {
        if (hit.button || at.X < slot.X || at.X > slot.X + slot.Width) return;
        float share = (at.X - slot.X) / slot.Width;
        hit.button = button;
        hit.id = ButtonAppId(button, false);
        hit.middle = share >= kMergeEdge && share <= 1 - kMergeEdge;
    });
    return hit;
}

// The loose folder takes no apps.
bool IsFolderTarget(const BarHit& hit) {
    return hit.middle && hit.id.starts_with(kAppIdPrefix) && !IsLooseId(hit.id);
}

// The gap between buttons nearest a hit: the app IDs left of it in taskbar order, and its
// middle on screen.
struct PinGap {
    std::vector<std::wstring> leftIds;
    POINT at{};
};

PinGap GapAt(const BarHit& hit) {
    std::vector<std::pair<float, std::wstring>> left;
    float gap = -1, next = std::numeric_limits<float>::infinity();
    float middle = float(hit.row.ActualHeight() / 2);
    ForEachButton(hit.row, [&](const wux::FrameworkElement& button, wf::Rect slot) {
        middle = slot.Y + slot.Height / 2;
        if (slot.X + slot.Width / 2 < hit.at.X) {
            left.emplace_back(slot.X, ButtonAppId(button, false));
            gap = std::max(gap, slot.X + slot.Width);
        } else {
            next = std::min(next, slot.X);
        }
    });
    std::sort(left.begin(), left.end());
    PinGap result;
    for (auto& [x, id] : left)
        if (!id.empty()) result.leftIds.push_back(std::move(id));
    float x = gap >= 0 ? gap : std::isfinite(next) ? next : hit.at.X;
    result.at = ToScreen(hit.space, {x, middle});
    return result;
}

// What is carried: a folder item, or a loose app (folder kLooseFolder).
struct CarryItem {
    std::wstring folder, path;
    LooseWindow loose;
};

// The merge look eases off, and the folder button `id` bounces: `hint` while it still is
// that button, else the first one found on a taskbar.
void SettleInto(const std::wstring& id, const winrt::weak_ref<wux::FrameworkElement>& hint) {
    ResetTouched(true);
    if (id.empty()) return;
    auto button = hint.get();
    if (button && ButtonAppId(button, false) != id) button = nullptr;
    for (HWND taskbar : button ? std::vector<HWND>{} : core::Taskbars())
        if (auto row = FindButtonRow(taskbar); row && (button = ButtonWithId(row, id))) break;
    if (button) Bounce(button);
}

// Worker: the item goes into the folder whose button is `id`, which then bounces (`button`
// is the one it was dropped on). A loose app with no item a shortcut can point at is
// pinned instead.
void CarryInto(const CarryItem& item, const std::wstring& id,
               const winrt::weak_ref<wux::FrameworkElement>& button) {
    bool loose = IsLoose(item.folder);
    std::wstring target = FolderFromAppId(id);
    std::vector<std::wstring> keys;
    bool moved = loose ? MoveLooseInto(item.loose, target, &keys)
                       : MoveInto(item.folder, item.path, target, &keys);
    RunOnTaskbar([id = moved ? id : std::wstring(), button] { SettleInto(id, button); });
    if (!moved && loose) PinLoose(item.loose);
    if (!keys.empty()) Regroup(keys);
}

// Worker, from the folder menu.
void MoveToFolderButton(const std::wstring& folder, const std::wstring& item, const std::wstring& id) {
    CarryInto({folder, item}, id, nullptr);
}

// From a worker: the app ID of the folder button whose middle is at `pt`, as the taskbar
// thread finds it; empty for anything else or when it does not answer in time.
std::wstring FolderButtonAt(POINT pt) {
    struct Answer {
        HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        std::wstring id;
        ~Answer() {
            if (done) CloseHandle(done);
        }
    };
    auto answer = std::make_shared<Answer>();
    if (!answer->done || !RunOnTaskbar([answer, pt] {
            try {
                HWND taskbar = TaskbarAt(pt);
                BarHit hit = taskbar ? HitRow(taskbar, FindButtonRow(taskbar), pt) : BarHit{};
                if (IsFolderTarget(hit)) answer->id = hit.id;
            } catch (...) {
            }
            SetEvent(answer->done);
        }))
        return {};
    return WaitForSingleObject(answer->done, 1000) == WAIT_OBJECT_0 ? answer->id : L"";
}

constexpr UINT_PTR kCarryTimer = 1;
constexpr UINT kCarryTickMs = 10;
constexpr float kCarryOpacity = 0.85f;  // as the held cell
constexpr DWORD kCarryStyle =
    WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

enum class Landing { Cancel, Into, Pin, TakeOut };

struct CarryPose {
    float scale = 1, opacity = kCarryOpacity;
};

struct CarryRow {
    HWND taskbar;
    winrt::weak_ref<wux::FrameworkElement> row;
    bool found = false;
};

struct Carry {
    HWND window = nullptr;
    CarryItem item;
    std::wstring sourceId;              // the folder button's app ID
    std::optional<POINT> home, beside;  // its middle and right edge on screen
    std::vector<HWND> taskbars;
    std::vector<CarryRow> rows;  // looked up once per taskbar
    Pixels icon;
    int iconSide = 0;  // of `icon`
    float side = 0;    // drawn side at the hand-over, in pixels
    wf::Point grab{};  // icon middle minus pointer at the hand-over, in pixels
    UINT dpi = 0;      // the window's DPI at the hand-over
    POINT pointer{};
    bool hold = true;  // keeps the capture
    winrt::weak_ref<wux::FrameworkElement> target;  // the folder button it is over
    std::wstring targetId;
    // The look eases from `from` to `to` over `ms` from `since`.
    CarryPose from, to;
    ULONGLONG since = 0;
    int ms = 0;
    bool landing = false;  // let go: it flies from `flyFrom` to `flyTo` and goes
    POINT flyFrom{}, flyTo{};
    bool shown = false;
    int drawnSide = 0;  // what the window shows now
    BYTE drawnAlpha = 0;
    POINT drawnAt{};
};
[[clang::no_destroy]] std::optional<Carry> g_carry;
WNDPROC g_carryBaseProc = nullptr;  // the STATIC class's, which the carry window keeps

float Progress(const Carry& c) {
    return c.ms > 0 ? std::min(1.0f, float(GetTickCount64() - c.since) / float(c.ms)) : 1.0f;
}

float Eased(float t) {  // decelerate, as the taskbar's motion
    float rest = 1 - t;
    return 1 - rest * rest * rest;
}

CarryPose PoseNow(const Carry& c) {
    float e = Eased(Progress(c));
    return {c.from.scale + (c.to.scale - c.from.scale) * e,
            c.from.opacity + (c.to.opacity - c.from.opacity) * e};
}

void Ease(Carry& c, CarryPose to, int ms) {
    c.from = PoseNow(c);
    c.to = to;
    c.since = GetTickCount64();
    c.ms = ms;
}

// Monitor DPI against the hand-over's: the icon keeps its size in DIPs.
double DpiRatio(const Carry& c) {
    UINT dpi = GetDpiForWindow(c.window);
    return c.dpi && dpi ? double(dpi) / c.dpi : 1;
}

POINT FollowCentre(const Carry& c) {
    double ratio = DpiRatio(c);
    return {LONG(std::lround(c.pointer.x + c.grab.X * ratio)),
            LONG(std::lround(c.pointer.y + c.grab.Y * ratio))};
}

// Premultiplied BGRA `src` (`from` square) resampled to `to` square, bilinear.
void Resample(const uint32_t* src, int from, uint32_t* dst, int to) {
    double step = double(from) / to;
    auto axis = [&](int i, int* a, int* b, double* f) {
        double s = std::clamp((i + 0.5) * step - 0.5, 0.0, from - 1.0);
        *a = int(s);
        *b = std::min(*a + 1, from - 1);
        *f = s - *a;
    };
    for (int y = 0; y < to; y++) {
        int y0, y1;
        double fy;
        axis(y, &y0, &y1, &fy);
        for (int x = 0; x < to; x++) {
            int x0, x1;
            double fx;
            axis(x, &x0, &x1, &fx);
            uint32_t out = 0;
            for (int shift = 0; shift < 32; shift += 8) {
                auto at = [&](int px, int py) { return double((src[py * from + px] >> shift) & 0xFF); };
                double v = (at(x0, y0) * (1 - fx) + at(x1, y0) * fx) * (1 - fy) +
                           (at(x0, y1) * (1 - fx) + at(x1, y1) * fx) * fy;
                out |= uint32_t(std::clamp(std::lround(v), 0L, 255L)) << shift;
            }
            dst[y * to + x] = out;
        }
    }
}

// The icon, `side` pixels square around `centre`, through the layered window. Following
// the pointer at a steady size only moves the window.
void Paint(Carry& c, POINT centre, int side, BYTE alpha) {
    if (!c.iconSide) return;  // no icon was read: nothing to show
    POINT topLeft{centre.x - side / 2, centre.y - side / 2};
    // Over the taskbar, which comes to the top as it is activated.
    bool raise = !c.shown || GetTopWindow(nullptr) != c.window;
    if (c.shown && side == c.drawnSide && alpha == c.drawnAlpha) {
        if (raise || topLeft.x != c.drawnAt.x || topLeft.y != c.drawnAt.y)
            SetWindowPos(c.window, raise ? HWND_TOPMOST : nullptr, topLeft.x, topLeft.y, 0, 0,
                         SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | (raise ? 0 : SWP_NOZORDER));
        c.drawnAt = topLeft;
        return;
    }
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), side, -side, 1, 32, BI_RGB};
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) return;
    Resample(c.icon.data(), c.iconSide, static_cast<uint32_t*>(bits), side);
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ old = SelectObject(dc, bitmap);
    POINT source{};
    SIZE size{side, side};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(c.window, nullptr, &topLeft, &size, dc, &source, 0, &blend, ULW_ALPHA);
    SelectObject(dc, old);
    DeleteDC(dc);
    DeleteObject(bitmap);
    if (raise)
        SetWindowPos(c.window, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    c.shown = true;
    c.drawnSide = side;
    c.drawnAlpha = alpha;
    c.drawnAt = topLeft;
}

// Draws the icon where it is now; true once its motion is over.
bool Draw(Carry& c) {
    float t = Progress(c), e = Eased(t);
    CarryPose pose = PoseNow(c);
    POINT centre = c.landing ? POINT{LONG(std::lround(c.flyFrom.x + (c.flyTo.x - c.flyFrom.x) * e)),
                                     LONG(std::lround(c.flyFrom.y + (c.flyTo.y - c.flyFrom.y) * e))}
                             : FollowCentre(c);
    int side = std::max(1, int(std::lround(c.side * DpiRatio(c) * pose.scale)));
    Paint(c, centre, side, BYTE(std::lround(std::clamp(pose.opacity, 0.0f, 1.0f) * 255)));
    return t >= 1;
}

// The window goes, with its timer and capture; a carry that never let go takes its merge
// look along. Re-entry safe: the window's messages find no carry from here on.
void EndCarry() {
    if (!g_carry) return;
    HWND window = g_carry->window;
    bool held = !g_carry->landing;
    g_carry.reset();
    if (held) ResetTouched(true);
    KillTimer(window, kCarryTimer);
    if (GetCapture() == window) ReleaseCapture();
    DestroyWindow(window);
}

wux::FrameworkElement RowOf(Carry& c, HWND taskbar) {
    auto it = std::find_if(c.rows.begin(), c.rows.end(),
                           [&](const CarryRow& r) { return r.taskbar == taskbar; });
    if (it == c.rows.end()) {
        it = c.rows.insert(c.rows.end(), CarryRow{taskbar});
    } else if (!it->found) {
        return nullptr;
    } else if (auto row = it->row.get()) {
        return row;
    }
    auto row = FindButtonRow(taskbar);  // first use, or the row went with a relayout
    it->row = row ? winrt::make_weak(row) : nullptr;
    it->found = bool(row);
    return row;
}

// A taskbar it cannot read counts as no taskbar; the carry goes on. Only the part of a
// taskbar's window on its own monitor counts: a hidden top one reaches the monitor above.
BarHit CarryHit(Carry& c, POINT pt) {
    for (HWND taskbar : c.taskbars) {
        RECT rect;
        MONITORINFO monitor{sizeof(monitor)};
        if (IsWindowVisible(taskbar) && GetWindowRect(taskbar, &rect) && PtInRect(&rect, pt) &&
            GetMonitorInfoW(core::MonitorOf(taskbar), &monitor) && PtInRect(&monitor.rcMonitor, pt)) {
            try {
                return HitRow(taskbar, RowOf(c, taskbar), pt);
            } catch (...) {
                return {};
            }
        }
    }
    return {};
}

Landing LandingAt(const Carry& c, const BarHit& hit) {
    if (hit.button && !c.sourceId.empty() && _wcsicmp(hit.id.c_str(), c.sourceId.c_str()) == 0)
        return Landing::Cancel;  // its own folder
    if (IsFolderTarget(hit)) return Landing::Into;
    return hit.row ? Landing::Pin : Landing::TakeOut;
}

// The folder button it is over shows the merge look, and the icon settles into it.
void SetCarryTarget(Carry& c, const wux::FrameworkElement& button, const std::wstring& id) {
    if (id == c.targetId) return;
    try {
        if (auto old = c.target.get()) ShowTargetLook(old, false);
        if (button) ShowTargetLook(button, true);
    } catch (...) {
    }
    c.target = button ? winrt::make_weak(button) : nullptr;
    c.targetId = id;
    Ease(c, button ? CarryPose{kDraggedOverPlate, kDraggedOpacity} : CarryPose{}, kFastMs);
}

void LetGo(Carry& c, bool cancel) {
    if (!cancel && !GetCursorPos(&c.pointer)) cancel = true;
    BarHit hit = cancel ? BarHit{} : CarryHit(c, c.pointer);
    Landing landing = cancel ? Landing::Cancel : LandingAt(c, hit);
    Wh_Log(L"carry let go: %d on %s", int(landing), hit.id.c_str());
    POINT here = FollowCentre(c), to = here;
    float scale = 0.6f;
    const CarryItem& item = c.item;
    auto pin = [&](std::optional<std::vector<std::wstring>> leftOf) {
        StartJob([item, leftOf] {
            if (IsLoose(item.folder))
                PinLoose(item.loose, leftOf);
            else
                TakeOut(item.folder, item.path, leftOf);
        });
    };
    if (landing == Landing::Into) {
        SetCarryTarget(c, hit.button, hit.id);
        StartJob([item, id = hit.id, button = winrt::make_weak(hit.button)] { CarryInto(item, id, button); });
        scale = 0.4f;
        try {
            if (auto space = SpaceOf(hit.taskbar, hit.button))
                to = ToScreen(*space, {float(hit.button.ActualWidth() / 2), float(hit.button.ActualHeight() / 2)});
        } catch (...) {
        }
    } else {
        SetCarryTarget(c, nullptr, L"");
        std::optional<PinGap> gap;
        try {
            if (landing == Landing::Pin) gap = GapAt(hit);
        } catch (...) {
            landing = Landing::TakeOut;  // the gap is unknown: next to the folder
        }
        if (landing == Landing::Cancel) {
            to = c.home.value_or(here);
        } else if (gap) {
            to = gap->at;
            scale = 0.8f;
            pin(std::move(gap->leftIds));
        } else {
            to = c.beside.value_or(here);
            scale = 0.8f;
            pin(std::nullopt);
        }
    }
    if (GetCapture() == c.window) ReleaseCapture();
    c.landing = true;
    c.flyFrom = here;
    c.flyTo = to;
    Ease(c, {scale, 0}, kNormalMs);
    Draw(c);
}

void FollowPointer(Carry& c) {
    POINT pt;
    if (!GetCursorPos(&pt)) return LetGo(c, true);  // the input went to another desktop
    // While it waits on a taskbar to come out, the taskbar moves under a still pointer.
    if (pt.x != c.pointer.x || pt.y != c.pointer.y || !c.hold) {
        c.pointer = pt;
        BarHit hit = CarryHit(c, pt);
        bool into = LandingAt(c, hit) == Landing::Into;
        SetCarryTarget(c, into ? hit.button : nullptr, into ? hit.id : L"");
        // Over a taskbar that is not all out, it needs the pointer to come out.
        c.hold = !hit.taskbar || hit.row;
        if (!c.hold && GetCapture() == c.window) ReleaseCapture();
    }
    Draw(c);
}

void CarryTick(Carry& c) {
    if (c.landing) {
        if (Draw(c)) EndCarry();
        return;
    }
    if (g_unloading || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) return LetGo(c, true);
    if (!PrimaryButtonDown()) return LetGo(c, false);  // released without a message to us
    FollowPointer(c);
    if (!c.landing && c.hold && GetCapture() != c.window) SetCapture(c.window);
}

LRESULT CALLBACK CarryProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    if (!g_carry || g_carry->window != window)
        return CallWindowProcW(g_carryBaseProc, window, msg, wParam, lParam);
    try {
        Carry& c = *g_carry;
        switch (msg) {
            case WM_TIMER:
                if (wParam == kCarryTimer) CarryTick(c);
                return 0;
            case WM_MOUSEMOVE:
                if (!c.landing) FollowPointer(c);
                return 0;
            case WM_LBUTTONUP:
                if (!c.landing) LetGo(c, false);
                return 0;
            case WM_CANCELMODE:
                if (!c.landing) LetGo(c, true);
                return 0;
        }
    } catch (...) {
        Wh_Log(L"carry failed: %08X", winrt::to_hresult());
        EndCarry();
        return 0;
    }
    return CallWindowProcW(g_carryBaseProc, window, msg, wParam, lParam);
}

// The middle of a cell's icon, in the cell.
wf::Point IconMiddle(const wux::Controls::Grid& cell) {
    auto children = cell.Children();
    auto stack = children.Size() > 1 ? children.GetAt(1).try_as<wux::Controls::Panel>() : nullptr;
    auto image = stack && stack.Children().Size() ? stack.Children().GetAt(0).try_as<wux::FrameworkElement>()
                                                  : nullptr;
    if (!image) return {float(cell.Width() / 2), float(cell.Height() / 2)};
    auto at = image.TransformToVisual(cell).TransformPoint({0, 0});
    return {at.X + float(image.ActualWidth() / 2), at.Y + float(image.ActualHeight() / 2)};
}

// The held cell leaves the popup: its icon goes on at the same place and size, and the
// popup closes behind it. False leaves the drag to the popup (no mouse button held, say).
bool StartCarry(Popup& p, const wux::Controls::Grid& cell) {
    if (g_unloading || (g_carry && !g_carry->landing) || !p.dragging ||
        p.from >= p.model.items.size() || !PrimaryButtonDown())
        return false;
    POINT cursor;
    if (!GetCursorPos(&cursor)) return false;
    EndCarry();  // one still landing ends now
    HWND window = nullptr;
    try {
        const PopupItem& item = p.model.items[p.from];
        auto root = p.content.XamlRoot();
        double scale = (root ? root.RasterizationScale() : 1) * UiScale(p.look);  // pixels per DIP
        // The icon against the pointer: where it is in its cell against where the press was;
        // both moved by the same drag.
        wf::Point icon = IconMiddle(cell);
        wf::Point grab{float((wux::Controls::Canvas::GetLeft(cell) + icon.X - p.start.X) * scale),
                       float((wux::Controls::Canvas::GetTop(cell) + icon.Y - p.start.Y) * scale)};
        auto anchor = p.anchor.get();
        std::optional<BarSpace> space = anchor ? SpaceOf(p.taskbar, anchor) : std::nullopt;
        std::wstring sourceId = anchor ? ButtonAppId(anchor, false)
                                : IsLoose(p.model.folder) ? L"" : FolderAppId(p.model.folder);

        window = CreateWindowExW(kCarryStyle, L"STATIC", nullptr, WS_POPUP, LONG(cursor.x + grab.X),
                                 LONG(cursor.y + grab.Y), 1, 1, nullptr, nullptr, nullptr, nullptr);
        if (!window) return false;
        g_carryBaseProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(CarryProc)));
        Carry& c = g_carry.emplace();
        c.window = window;
        c.item = {p.model.folder, item.path, item.loose};
        c.sourceId = sourceId;
        if (space) {
            float w = float(anchor.ActualWidth()), h = float(anchor.ActualHeight());
            c.home = ToScreen(*space, {w / 2, h / 2});
            c.beside = ToScreen(*space, {w, h / 2});
        }
        c.taskbars = core::Taskbars();
        if (item.iconSize > 0 && item.icon.size() == size_t(item.iconSize) * item.iconSize) {
            c.icon = item.icon;
            c.iconSide = item.iconSize;
        }
        c.side = float(p.look.iconSize * scale);
        c.grab = grab;
        c.pointer = cursor;
        c.dpi = GetDpiForWindow(window);
        SetTimer(window, kCarryTimer, kCarryTickMs, nullptr);
        Wh_Log(L"carry out of %s", p.model.folder.c_str());
    } catch (...) {
        Wh_Log(L"carry failed to start: %08X", winrt::to_hresult());
        if (g_carry) {
            EndCarry();
        } else if (window) {
            DestroyWindow(window);
        }
        return false;
    }
    // The cell is now the icon outside; the popup's own drag ends without a drop.
    p.down = p.dragging = false;
    try {
        cell.Opacity(0);
        cell.ReleasePointerCaptures();
    } catch (...) {
    }
    if (g_carry) {
        SetCapture(g_carry->window);
        Draw(*g_carry);
    }
    try {
        HideSelf(p);
    } catch (...) {
    }
    return true;
}

// ---- Drops from outside (Taskbar.View) ------------------------------------------

// Files dragged from Explorer onto a folder's button go into the folder; without this the
// taskbar would hand them to the pin (explorer) or pin them. The button's own XAML drag
// overrides get `this` as its IControlOverrides.

using DragOverride_t = int(WINAPI*)(void* pThis, void* args);
DragOverride_t OnDragEnter_Original, OnDragOver_Original, OnDrop_Original, OnDragLeave_Original;

wux::DragEventArgs DragArgs(void* args) {
    wux::DragEventArgs e{nullptr};
    winrt::copy_from_abi(e, args);
    return e;
}

// The folder of the button under a drag of files, or empty.
std::wstring DropFolder(void* pThis, const wux::DragEventArgs& e) {
    wux::FrameworkElement button{nullptr};
    static_cast<IUnknown*>(pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                                  winrt::put_abi(button));
    if (!button || winrt::get_class_name(button) != kTaskListButtonClass) return {};
    std::wstring id = ButtonAppId(button, false);
    if (!id.starts_with(kAppIdPrefix) ||
        !e.DataView().Contains(wadt::StandardDataFormats::StorageItems()))
        return {};
    // Drag-over comes with every pointer move: the folder is looked up once per button.
    static std::wstring lastId, lastFolder;  // taskbar thread only
    if (id != lastId) {
        lastId = id;
        lastFolder = FolderFromAppId(id);
        Wh_Log(L"drag of files over %s", lastFolder.c_str());
    }
    return lastFolder;
}

// Over a folder's button the drop is taken as a link, "Into <folder>" on the cursor.
void AcceptFolderDrag(void* pThis, void* args) {
    if (g_unloading) return;
    try {
        wux::DragEventArgs e = DragArgs(args);
        std::wstring folder = DropFolder(pThis, e);
        if (folder.empty()) return;
        e.AcceptedOperation(wadt::DataPackageOperation::Link);
        if (auto ui = e.DragUIOverride()) ui.Caption(FolderName(folder) + L"에 넣기");
        e.Handled(true);
    } catch (...) {
    }
}

int WINAPI OnDragEnter_Hook(void* pThis, void* args) {
    int hr = OnDragEnter_Original(pThis, args);
    AcceptFolderDrag(pThis, args);
    return hr;
}

int WINAPI OnDragOver_Hook(void* pThis, void* args) {
    int hr = OnDragOver_Original(pThis, args);
    AcceptFolderDrag(pThis, args);
    return hr;
}

// Hooked for its original, which a drop on a folder calls to end the taskbar's drag.
int WINAPI OnDragLeave_Hook(void* pThis, void* args) {
    return OnDragLeave_Original(pThis, args);
}

int WINAPI OnDrop_Hook(void* pThis, void* args) {
    if (!g_unloading) {
        try {
            NoteTaskbarThread();
            wux::DragEventArgs e = DragArgs(args);
            if (std::wstring folder = DropFolder(pThis, e); !folder.empty()) {
                e.AcceptedOperation(wadt::DataPackageOperation::Link);
                e.Handled(true);
                wux::FrameworkElement button{nullptr};
                static_cast<IUnknown*>(pThis)->QueryInterface(
                    winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(button));
                Wh_Log(L"drop onto %s", folder.c_str());
                StartJob([folder, items = winrt::make_agile(e.DataView().GetStorageItemsAsync()),
                          weak = winrt::make_weak(button)] {
                    std::vector<std::wstring> paths;
                    try {
                        for (const auto& item : items.get().get())
                            if (!item.Path().empty()) paths.emplace_back(item.Path());
                    } catch (...) {
                        Wh_Log(L"dropped items unreadable: %08X", winrt::to_hresult());
                    }
                    Wh_Log(L"dropped %d files", int(paths.size()));
                    if (DropInto(folder, paths))
                        RunOnTaskbar([weak] {
                            if (auto button = weak.get()) Bounce(button);
                        });
                });
                // The taskbar's own drag state ends as if the drag had left the button.
                return OnDragLeave_Original(pThis, args);
            }
        } catch (...) {
            Wh_Log(L"drop failed: %08X", winrt::to_hresult());
        }
    }
    return OnDrop_Original(pThis, args);
}

// ---- Folder button look (Taskbar.View) -------------------------------------------

// Windows draws a button with several windows as a stack of plates. A folder's button
// holds several apps' windows, and looks like a button with one instead. Only the
// button's look reads the group through this getter; clicks and previews ask it directly.
using GroupGetBool_t = HRESULT(WINAPI*)(void* pThis, bool* value);
using GroupGetString_t = HRESULT(WINAPI*)(void* pThis, void** value);
GroupGetBool_t GroupIsMultiWindow_Original;
GroupGetString_t GroupAutomationId;  // found by symbol only, never hooked

HRESULT WINAPI GroupIsMultiWindow_Hook(void* pThis, bool* value) {
    HRESULT hr = GroupIsMultiWindow_Original(pThis, value);
    if (FAILED(hr) || !*value || !GroupAutomationId) return hr;
    winrt::hstring id;
    if (SUCCEEDED(GroupAutomationId(pThis, winrt::put_abi(id))) &&
        AppIdFromAutomationId(id).starts_with(kAppIdPrefix))
        *value = false;
    return hr;
}

bool IsFolderButton(const wux::FrameworkElement& button) {
    return ButtonAppId(button, false).starts_with(kAppIdPrefix);  // the loose folder's too
}

// A folder's button shows no running line. The taskbar makes Rectangle#RunningIndicator
// only while a button has windows, so each new line is hidden on the button's state pass
// (hooks, UpdateVisualStates). Its states change only its Fill, but the button sets its
// Opacity to 1 again as it loads or enters, which the watch takes back.
// shortcut: a reused button whose app ID comes after its state pass keeps the old line
// state until its next pass; watch its AutomationId if that ever shows.
void HideRunningLine(const wux::FrameworkElement& button) {
    auto line = FindChild(IconPanel(button), L"RunningIndicator", 1);
    if (!line) return;
    auto opacity = wux::UIElement::OpacityProperty();
    if (IsFolderButton(button)) {
        if (line.Opacity() != 0) core::Set(line, opacity, winrt::box_value(0.0));
        core::Watch(line, opacity, [opacity, weak = winrt::make_weak(button)](const wux::DependencyObject& sender) {
            auto button = weak.get();
            if (button && IsFolderButton(button) && sender.as<wux::UIElement>().Opacity() != 0)
                sender.SetValue(opacity, winrt::box_value(0.0));
        });
    } else if (line.Opacity() == 0) {
        line.ClearValue(opacity);  // a reused button that was a folder's
    }
}

// The buttons that exist, as the folders start or come back after a settings change.
void HideRunningLines(HWND taskbar) {
    auto root = core::TaskbarRoot(taskbar);
    auto content = root ? root.Content().try_as<wux::FrameworkElement>() : nullptr;
    if (!content) return;
    core::FindDescendant(
        content,
        [](auto& e) {
            if (winrt::get_class_name(e) == kTaskListButtonClass)
                core::Guard(L"Folder line", [&] { HideRunningLine(e); });
            return false;
        },
        40);
}

// Every symbol is optional: one that is missing costs only its own part of the folders,
// never the rest of the mod. Without the drag hooks only the folder menu works.
void CollectTaskbarView(std::vector<WindhawkUtils::SYMBOL_HOOK>& list) {
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragStartedGesture(struct winrt::Windows::Foundation::Point))"},
                    &OnDragStartedGesture_Original, OnDragStartedGesture_Hook, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateDrag(struct winrt::Windows::Foundation::Point))"},
                    &UpdateDrag_Original, UpdateDrag_Hook, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"},
                    &OnDragCompletedGesture_Original, OnDragCompletedGesture_Hook, true});
    list.push_back({{LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *,bool))",
                    // Older builds lack the bool; the extra argument is harmless on x64.
                    LR"(public: struct winrt::Taskbar::implementation::DropPlaceholder __cdecl winrt::Taskbar::implementation::TaskListDragOperation::GetDropPlaceholder(class std::vector<struct winrt::Windows::Foundation::Rect,class std::allocator<struct winrt::Windows::Foundation::Rect> > const &,struct std::pair<unsigned int,unsigned int>,struct winrt::Windows::Foundation::Rect,enum winrt::Taskbar::implementation::RearrangeDirection,struct winrt::Taskbar::implementation::GroupBoundsCalculator const *))"},
                    &GetDropPlaceholder_Original, GetDropPlaceholder_Hook, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::PlayEntranceAnimation(enum winrt::Taskbar::implementation::TaskListButtonEntranceAnimationKind,double,struct winrt::Windows::Foundation::Numerics::float3,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >,bool))"},
                    &PlayEntranceAnimation_Original, PlayEntranceAnimation_Hook, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::StartExitAnimation(void))"},
                    &StartExitAnimation_Original, StartExitAnimation_Hook, true});
    list.push_back({{LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListButton::Invoke(struct winrt::Windows::UI::Input::PointerPoint const &,class std::optional<enum winrt::Windows::System::VirtualKey>))"},
                    &Invoke_Original, Invoke_Hook, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
                    &OnTaskListButtonContextRequested_Original, OnTaskListButtonContextRequested_Hook, true});
    list.push_back({{LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
                    &HandleContextRequested_Original, HandleContextRequested_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnDragEnter(void *))"},
                    &OnDragEnter_Original, OnDragEnter_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnDragOver(void *))"},
                    &OnDragOver_Original, OnDragOver_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnDragLeave(void *))"},
                    &OnDragLeave_Original, OnDragLeave_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnDrop(void *))"},
                    &OnDrop_Original, OnDrop_Hook, true});
    list.push_back({{LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
                    &TryGetGroupViewModel, nullptr, true});
    list.push_back({{LR"(public: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::Dismiss(bool))"},
                    &HoverFlyoutDismiss, nullptr, true});
    list.push_back({{LR"(public: bool __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::IsFlyoutVisible(void)const )"},
                    &HoverFlyoutVisible, nullptr, true});
    list.push_back({{LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::CommitDismissFlyout(void))"},
                    &CommitDismissFlyout_Original, CommitDismissFlyout_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_IsMultiWindow(bool *))"},
                    &GroupIsMultiWindow_Original, GroupIsMultiWindow_Hook, true});
    list.push_back({{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::get_AutomationId(void * *))"},
                    &GroupAutomationId, nullptr, true});
}

void TaskbarViewHooked() {
    Wh_Log(L"Taskbar.View hooks: drag %d, click %d, previews close: %d, visible: %d, gone: %d, "
           L"one-window look: %d",
           OnDragStartedGesture_Original && UpdateDrag_Original && OnDragCompletedGesture_Original,
           Invoke_Original != nullptr, TryGetGroupViewModel && HoverFlyoutDismiss,
           HoverFlyoutVisible != nullptr, CommitDismissFlyout_Original != nullptr,
           GroupIsMultiWindow_Original && GroupAutomationId);
}

// Taskbar.dll groups windows into buttons; without these hooks folder apps keep
// their own buttons.
void CollectTaskbarDll(std::vector<WindhawkUtils::SYMBOL_HOOK>& list) {
    list.push_back({{LR"(protected: void __cdecl CTaskBand::_HandleItemResolved(struct RESOLVEDWINDOW *,struct ITaskListUI *,struct ITaskGroup *,struct ITaskItem *))"},
                    &HandleItemResolved_Original, HandleItemResolved_Hook, true});
    list.push_back({{LR"(protected: void __cdecl CTaskBand::_HandleWindowResolved(struct RESOLVEDWINDOW *))"},
                    &HandleWindowResolved_Original, HandleWindowResolved_Hook, true});
    list.push_back({{LR"(protected: void __cdecl CTaskBand::HandleTaskGroupSwitchItemAdded(struct winrt::Windows::Internal::ComposableShell::Multitasking::ISwitchItem const &))"},
                    &HandleTaskGroupSwitchItemAdded_Original, HandleTaskGroupSwitchItemAdded_Hook, true});
}

void TaskbarDllHooked() {
    Wh_Log(L"Taskbar.dll hooks: %d, window resolve: %d", HandleItemResolved_Original != nullptr,
           HandleWindowResolved_Original != nullptr);
}

// ---- Launch hook and mod lifecycle ----------------------------------------

using CreateProcessInternalW_t = BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR,
                                               LPSECURITY_ATTRIBUTES,
                                               LPSECURITY_ATTRIBUTES, BOOL, DWORD,
                                               LPVOID, LPCWSTR, LPSTARTUPINFOW,
                                               LPPROCESS_INFORMATION, PHANDLE);
CreateProcessInternalW_t CreateProcessInternalW_Original;

BOOL WINAPI CreateProcessInternalW_Hook(HANDLE token, LPCWSTR app, LPWSTR cmd,
                                        LPSECURITY_ATTRIBUTES processAttributes,
                                        LPSECURITY_ATTRIBUTES threadAttributes,
                                        BOOL inheritHandles, DWORD flags, LPVOID env,
                                        LPCWSTR dir, LPSTARTUPINFOW startupInfo,
                                        LPPROCESS_INFORMATION processInfo,
                                        PHANDLE newToken) {
    std::wstring folder = PinnedFolderFromCommand(cmd, Root());
    if (folder.empty())
        return CreateProcessInternalW_Original(token, app, cmd, processAttributes,
                                               threadAttributes, inheritHandles, flags,
                                               env, dir, startupInfo, processInfo,
                                               newToken);
    Wh_Log(L"Pin launched: %s", folder.c_str());
    OnPinLaunched(folder);
    SetLastError(ERROR_CANCELLED);  // the shell shows no error for a cancel
    return FALSE;
}

void ApplySettings() {
    const auto& f = settings::g.folders;
    std::wstring root = ExpandPath(f.path.c_str());
    g_mergeDelayMs = f.mergeDelay;
    Look look;
    look.labels = f.showLabels;
    look.iconSize = f.iconSize;
    look.spacing = f.appSpacing;
    look.gap = f.popupGap;
    look.radius = f.cornerRadius;
    look.smoothing = f.cornerSmoothing;
    look.scale = f.popupScale;
    look.side = f.popupSide == settings::FolderSide::kAbove   ? Look::Side::Above
                : f.popupSide == settings::FolderSide::kBelow ? Look::Side::Below
                                                              : Look::Side::Auto;
    AcquireSRWLockExclusive(&g_lock);
    g_look = look;
    g_root = root;
    g_looseOn = f.autoRunning;
    g_looseName = IsValidFolderName(f.runningName) ? f.runningName : kLooseName;
    ReleaseSRWLockExclusive(&g_lock);
    if (root.empty()) return;
    CreateDirectoryW(root.c_str(), nullptr);
    CreateDirectoryW((root + L"\\" + kPinsDir).c_str(), nullptr);
}

// Windhawk thread, at init: the settings, then the launch hook.
bool Install() {
    ApplySettings();
    core::g_popupDropsTimer = PopupDropsTimer;  // before the taskbars are subclassed
    auto createProcess = reinterpret_cast<CreateProcessInternalW_t>(
        GetProcAddress(GetModuleHandleW(L"kernelbase.dll"), "CreateProcessInternalW"));
    return createProcess &&
           WindhawkUtils::SetFunctionHook(createProcess, CreateProcessInternalW_Hook,
                                          &CreateProcessInternalW_Original);
}

// Windows that were open before the mod: folder apps' ones join their folders.
// On an update the old copy sends them out of their folders just before this, and two
// changes that close together leave only the first; the old copy marks the taskbar's
// window with when it did, and the start waits until a second after that, so the
// taskbar takes that one first. No mark: no copy let windows go in this explorer (it is
// starting, or this is the first load), and nothing needs to settle.
// shortcut: a copy from before the mark leaves none, so the first update from one does
// not wait; Regroup's second go takes the windows the taskbar drops then.
void BeginStart(bool afterUpdate) {
    StartJob([afterUpdate] {
        HWND tray = afterUpdate ? FindWindowW(L"Shell_TrayWnd", nullptr) : nullptr;
        ULONGLONG released = tray ? ULONG_PTR(RemovePropW(tray, kReleasedAtProp)) : 0;
        Wh_Log(L"folders start%s", released ? L" after the old copy's release" : L"");
        while (released && GetTickCount64() < released + 1000 && !g_unloading) Sleep(50);
        if (g_unloading) return;
        for (const Pin& pin : ReadPins())
            if (std::wstring folder = FolderOfPin(pin); !folder.empty()) RestorePinLink(pin, folder);
        RebuildMembers();
        Regroup(MemberKeys());
        // Folders first: a pin made again comes after the loose folder, which then goes last.
        UpdateLoose(RethemeIcons());
    });
}

// Only explorer's taskbar process has the windows (a second explorer may run folder
// windows). One that is starting up has no taskbar yet; its resolve hook starts later.
void Start() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) {
        g_startPending = true;
        return;
    }
    DWORD process = 0;
    GetWindowThreadProcessId(tray, &process);
    if (process == GetCurrentProcessId()) BeginStart(true);
}

void SettingsChanged() {
    ApplySettings();
    StartJob([] {
        RebuildMembers();
        UpdateLoose(RethemeIcons());
    });
}

// Windhawk thread, at unload, while the hooks still run.
void Stop() {
    g_unloading = true;
    // While the hook still runs: grouped windows get their own buttons back.
    ReleaseGroupedWindows();
    // The loose folder's files go with it; a worker still on them leaves them to the
    // next start, which tidies up.
    if (TryAcquireSRWLockExclusive(&g_looseWork)) {
        CleanLooseDir(g_looseDir, {});
        ReleaseSRWLockExclusive(&g_looseWork);
    }
    // Taskbar-thread state holds delegates into this module: undo it there, and wait.
    HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!done) return;
    bool queued = RunOnTaskbar(
        [done] {
            EndDrag();
            EndCarry();
            ResetTouched(false);
            core::g_popupDropsTimer = nullptr;  // the tray's own check takes the taskbar in
            DropPreviewWait();
            DropGiveBack();
            ClosePopup();
            SetEvent(done);
        },
        true);
    if (queued && WaitForSingleObject(done, 2000) != WAIT_OBJECT_0)
        Wh_Log(L"taskbar cleanup timed out");  // keep the event: the cleanup may still set it
    else
        CloseHandle(done);
}

// Wh_ModUninit: workers call Windhawk and shell APIs, and queued taskbar work runs module
// code; let both finish while the mod is alive.
void WaitForWork() {
    for (int waited = 0; (g_jobs > 0 || g_taskbarWork > 0) && waited < 3000; waited += 50) {
        CloseMenus();
        Sleep(50);
    }
    if (g_jobs > 0 || g_taskbarWork > 0)
        Wh_Log(L"%d jobs, %d taskbar calls still pending at unload", g_jobs.load(),
               g_taskbarWork.load());
    // shortcut: the last counted callable may still be returning from its destructor;
    // a short grace covers that. A module reference held per call would close it for good.
    Sleep(100);
}

}  // namespace folders

// ============================================================================
namespace ai_sessions {
// ============================================================================
// What Claude Code and Codex are doing, from their own session logs on this PC,
// for the usage details: each recent session's state, its turn's output tokens
// and how full its context is. The reading follows TokenCat's (MIT, see
// THIRD_PARTY.md), cut down: only the tail of each log, and of it only usage
// numbers, times, ids, titles and model names, never what was said. Nothing is
// read unless the details are open. A worker reads (one at a time); the rows go
// to the taskbar thread.

namespace wdj = winrt::Windows::Data::Json;

enum class Source { kClaude, kCodex };
// What a session is doing; kOutput is TokenCat's, shown as working.
enum class State { kIdle, kWorking, kOutput, kTool, kInput, kStale, kComplete, kInterrupted, kUnfinished };

constexpr uint64_t kTailBytes = 256 * 1024;      // from the end, on a log's first look
constexpr uint64_t kHeadBytes = 64 * 1024;       // a Codex log's own session record
constexpr uint64_t kReadBudget = 1024 * 1024;    // per log per look
constexpr size_t kMaxLine = 256 * 1024;          // longer lines only end a tool call
constexpr size_t kPrefixBytes = 16 * 1024;       // of a longer line, for that
constexpr size_t kPerSource = 6;                 // newest logs read per tool
constexpr int64_t kRecentSeconds = 24 * 3600;    // older logs are left out
constexpr int kFindEvery = 5;                    // looks between two searches for logs
constexpr size_t kMaxSet = 512;                  // ids remembered per log
// Subagents: only those of the sessions read, written in the last hour, from a
// shorter tail.
constexpr uint64_t kAgentTailBytes = 64 * 1024;
constexpr int64_t kAgentSeconds = 3600;
constexpr size_t kAgentsPerSession = 8, kMaxAgents = 16;

// ---- a log's state (worker) ----------------------------------------------------

struct PendingTool {
    std::wstring id, name;
};

int64_t Unix(FILETIME const& t) {
    ULARGE_INTEGER u{{t.dwLowDateTime, t.dwHighDateTime}};
    return (int64_t)(u.QuadPart / 10000000ULL) - 11644473600LL;
}

// Days from 1970-01-01 to a civil date (H. Hinnant's days_from_civil).
int64_t DaysFromCivil(int64_t y, int64_t m, int64_t d) {
    y -= m <= 2;
    int64_t era = (y >= 0 ? y : y - 399) / 400;
    int64_t yoe = y - era * 400;
    int64_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    int64_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

// "2026-11-09T01:23:45.678Z" or with an offset, as Unix seconds; 0 when it isn't one.
int64_t IsoTime(std::wstring_view text) {
    auto digits = [&](size_t at, size_t n, int& out) {
        out = 0;
        for (size_t i = at; i < at + n; i++) {
            if (i >= text.size() || !iswdigit(text[i])) {
                return false;
            }
            out = out * 10 + (text[i] - L'0');
        }
        return true;
    };
    int y, mo, d, h, mi, s;
    if (!digits(0, 4, y) || !digits(5, 2, mo) || !digits(8, 2, d) || !digits(11, 2, h) ||
        !digits(14, 2, mi) || !digits(17, 2, s) || text[4] != L'-' || text[7] != L'-' ||
        text[10] != L'T' || mo < 1 || mo > 12 || d < 1 || d > 31) {
        return 0;
    }
    size_t zone = 19;
    while (zone < text.size() && (text[zone] == L'.' || iswdigit(text[zone]))) {
        zone++;
    }
    int offset = 0;
    if (zone < text.size() && (text[zone] == L'+' || text[zone] == L'-')) {
        int oh, om;
        if (!digits(zone + 1, 2, oh) || !digits(zone + 4, 2, om)) {
            return 0;
        }
        offset = (text[zone] == L'-' ? -1 : 1) * (oh * 3600 + om * 60);
    } else if (zone >= text.size() || text[zone] != L'Z') {
        return 0;
    }
    int64_t at = DaysFromCivil(y, mo, d) * 86400 + h * 3600 + mi * 60 + s - offset;
    return at > 0 ? at : 0;
}

// A session title as a line: control characters become spaces, runs of space
// one, at most 80 characters.
std::wstring CleanTitle(std::wstring_view text) {
    std::wstring out;
    for (WCHAR c : text) {
        bool space = c < 0x20 || c == 0x7F || iswspace(c);
        if (space) {
            if (!out.empty() && out.back() != L' ') {
                out += L' ';
            }
        } else {
            out += c;
        }
    }
    while (!out.empty() && out.back() == L' ') {
        out.pop_back();
    }
    if (out.size() > 80) {
        out = out.substr(0, 79) + L"…";
    }
    return out;
}

// A bounded set of ids: forgets everything once full, as TokenCat's caps do.
struct Seen {
    std::unordered_set<std::wstring> ids;
    // False when `id` was there already.
    bool Insert(std::wstring const& id) {
        if (ids.size() >= kMaxSet) {
            ids.clear();
        }
        return ids.insert(id).second;
    }
};

struct Log {
    Source source = Source::kClaude;
    std::wstring path;
    // A subagent's log: its id, its kind (Claude's agentType, Codex's role or
    // nickname) and the session it works for.
    bool subagent = false;
    std::wstring agentId, role, parentId;
    int roleLooks = 0;  // Claude writes the kind beside the log, maybe later
    // Where the next read starts; the file's creation time, to see it replaced.
    uint64_t offset = 0;
    int64_t created = 0;
    bool started = false;
    std::string partial;  // an unfinished last line

    std::wstring sessionId, project, projectPath, model, effort;
    std::wstring customTitle, aiTitle, summary;  // Claude, in that order of rank

    // The turn.
    int64_t startedAt = 0;  // its prompt's time, when it was seen
    bool metadataOpen = false, softClosed = false;
    std::wstring turnId, metadataTurnId;
    int64_t activityStartedAt = 0;
    State observed = State::kIdle;
    std::vector<PendingTool> pending;
    int output = 0;
    bool hasUsage = false, accurate = true;
    std::optional<int> cumulative;            // Codex: the total output so far
    std::optional<std::pair<std::wstring, int>> turnUsage;  // Codex: the turn's total
    bool usageRecords = false;                // Codex: token_usage_record lines
    std::unordered_map<std::wstring, int> messages;  // Claude: output per message id
    Seen previousMessages, records, responses, closedTurns;
    // Codex: a fork replays its parent's turns before its own; those are skipped.
    int64_t sessionCreated = 0;
    bool ownTurnSeen = false, ignoringInherited = false;
    std::wstring ignoredTurn;

    int64_t lastActivity = 0, lastLogAt = 0;
    int64_t lastOutputAt = 0;
    int lastOutputDelta = 0;
    int lastTurnOutput = 0;  // the last finished turn's
    int64_t contextAt = 0;
    int contextUsed = -1, contextWindow = 0;

    bool TurnOpen() const { return (startedAt || metadataOpen) && !softClosed; }
    int64_t LiveAt() const { return std::max(lastActivity, lastLogAt); }

    static bool IsInputTool(std::wstring const& name) {
        return name == L"AskUserQuestion" || name == L"ExitPlanMode" ||
               name == L"request_user_input";
    }

    bool WaitsForInput() const {
        return TurnOpen() && std::ranges::any_of(pending, [](PendingTool const& t) {
                   return IsInputTool(t.name);
               });
    }

    // How long an open turn may stay silent and still be running: a question
    // waits for the person; Claude's tools can run for minutes, Codex's yield soon.
    int64_t LiveHorizon() const {
        return WaitsForInput() ? 86400 : pending.empty() ? 600 : source == Source::kClaude ? 900 : 120;
    }

    bool IsActive(int64_t now) const {
        if (!TurnOpen() || !LiveAt()) {
            return false;
        }
        int64_t age = now - LiveAt();
        return age >= -5 && age <= LiveHorizon();
    }

    State Activity(int64_t now) const {
        if (!TurnOpen()) {
            // A turn whose start fell before the tail: running only while it writes.
            bool running = observed == State::kWorking || observed == State::kOutput ||
                           observed == State::kTool;
            return running && now - LiveAt() > 600 ? State::kIdle : observed;
        }
        if (WaitsForInput()) {
            return IsActive(now) ? State::kInput : State::kUnfinished;
        }
        if (IsActive(now)) {
            return observed;
        }
        return LiveAt() && now - LiveAt() <= 1800 ? State::kStale : State::kUnfinished;
    }

    // The tool that holds an open turn: one waiting for the person, else the newest.
    std::wstring RunningTool() const {
        for (auto it = pending.rbegin(); it != pending.rend(); ++it) {
            if (IsInputTool(it->name)) {
                return it->name;
            }
        }
        return pending.empty() ? L"" : pending.back().name;
    }

    std::optional<int> TurnOutput() const {
        if (!TurnOpen()) {
            return std::nullopt;
        }
        if (turnUsage && turnUsage->first == (!turnId.empty() ? turnId : metadataTurnId)) {
            return turnUsage->second;
        }
        return startedAt && accurate ? std::optional<int>(output) : std::nullopt;
    }

    std::wstring Title() const {
        return !customTitle.empty() ? customTitle : !aiTitle.empty() ? aiTitle : summary;
    }

    void SetProject(std::wstring const& cwd) {
        if (cwd.empty() || cwd == projectPath) {
            return;
        }
        projectPath = cwd;
        size_t end = cwd.find_last_not_of(L"\\/");
        size_t start = end == std::wstring::npos ? 0 : cwd.find_last_of(L"\\/", end);
        project = end == std::wstring::npos
                      ? cwd
                      : cwd.substr(start == std::wstring::npos ? 0 : start + 1,
                                   end - (start == std::wstring::npos ? 0 : start + 1) + 1);
    }

    void AddTool(std::wstring const& id, std::wstring const& name) {
        std::erase_if(pending, [&](PendingTool const& t) { return t.id == id; });
        pending.push_back({id, name});
        if (pending.size() > 256) {
            pending.erase(pending.begin());
        }
    }

    bool RemoveTool(std::wstring const& id) {
        auto it = std::ranges::find(pending, id, &PendingTool::id);
        if (it == pending.end()) {
            return false;
        }
        pending.erase(it);
        return true;
    }

    void RecordOutput(int tokens, int64_t at) {
        if (tokens <= 0) {
            return;
        }
        if (at) {
            lastActivity = std::max(lastActivity, at);
        }
        lastOutputAt = at;
        lastOutputDelta = tokens;
    }

    void Begin(std::wstring const& id, int64_t at) {
        startedAt = at;
        softClosed = metadataOpen = false;
        metadataTurnId.clear();
        activityStartedAt = at;
        observed = State::kWorking;
        lastOutputAt = 0;
        lastOutputDelta = 0;
        pending.clear();
        turnId = id;
        turnUsage.reset();
        output = 0;
        hasUsage = false;
        accurate = true;
        for (auto const& [message, count] : messages) {
            previousMessages.Insert(message);
        }
        messages.clear();
    }

    void RecordCompletion() {
        std::optional<int> total;
        if (turnUsage && turnUsage->first == (!turnId.empty() ? turnId : metadataTurnId)) {
            total = turnUsage->second;
        } else if (hasUsage && accurate && startedAt) {
            total = output;
        }
        if (total && *total > 0) {
            lastTurnOutput = *total;
        }
    }

    void CloseTurn(State state, int64_t at) {
        if (state == State::kComplete && (startedAt || metadataOpen)) {
            RecordCompletion();
        }
        startedAt = 0;
        turnId.clear();
        turnUsage.reset();
        softClosed = metadataOpen = false;
        metadataTurnId.clear();
        activityStartedAt = 0;
        observed = state;
        pending.clear();
        if (at) {
            lastActivity = std::max(lastActivity, at);
        }
    }
};

// ---- reading a record (worker) ----------------------------------------------------------

wdj::JsonObject Object(wdj::IJsonValue const& value) {
    return value && value.ValueType() == wdj::JsonValueType::Object ? value.GetObject() : nullptr;
}

wdj::JsonObject Child(wdj::JsonObject const& obj, PCWSTR key) {
    return obj ? Object(obj.TryLookup(key)) : nullptr;
}

std::wstring Str(wdj::JsonObject const& obj, PCWSTR key) {
    if (!obj) {
        return {};
    }
    auto value = obj.TryLookup(key);
    return value && value.ValueType() == wdj::JsonValueType::String
               ? std::wstring(value.GetString())
               : std::wstring();
}

bool True(wdj::JsonObject const& obj, PCWSTR key) {
    if (!obj) {
        return false;
    }
    auto value = obj.TryLookup(key);
    return value && value.ValueType() == wdj::JsonValueType::Boolean && value.GetBoolean();
}

// A whole non-negative count, as TokenCat takes one.
std::optional<int> Count(wdj::JsonObject const& obj, PCWSTR key) {
    if (!obj) {
        return std::nullopt;
    }
    auto value = obj.TryLookup(key);
    if (!value || value.ValueType() != wdj::JsonValueType::Number) {
        return std::nullopt;
    }
    double n = value.GetNumber();
    return n >= 0 && n <= std::numeric_limits<int>::max() && std::trunc(n) == n
               ? std::optional<int>((int)n)
               : std::nullopt;
}

int Add(int a, int b) {
    return (int)std::min<int64_t>((int64_t)a + b, std::numeric_limits<int>::max());
}

int64_t TimeOf(wdj::JsonObject const& obj, PCWSTR key) {
    if (!obj) {
        return 0;
    }
    auto value = obj.TryLookup(key);
    if (!value) {
        return 0;
    }
    if (value.ValueType() == wdj::JsonValueType::String) {
        return IsoTime(std::wstring(value.GetString()));
    }
    if (value.ValueType() == wdj::JsonValueType::Number) {
        double s = value.GetNumber();
        return s > 0 && s < 253370764800.0 ? (int64_t)s : 0;
    }
    return 0;
}

// An array of objects, or none when anything in it isn't one.
std::vector<wdj::JsonObject> Objects(wdj::JsonObject const& obj, PCWSTR key) {
    std::vector<wdj::JsonObject> out;
    if (!obj) {
        return out;
    }
    auto value = obj.TryLookup(key);
    if (!value || value.ValueType() != wdj::JsonValueType::Array) {
        return out;
    }
    for (auto const& item : value.GetArray()) {
        if (item.ValueType() != wdj::JsonValueType::Object) {
            return {};
        }
        out.push_back(item.GetObject());
    }
    return out;
}

// A short client name (an effort level); anything else is dropped.
std::wstring Label(std::wstring const& text) {
    if (text.empty() || text.size() > 48) {
        return {};
    }
    for (WCHAR c : text) {
        if (!iswalnum(c) && c != L' ' && c != L'_' && c != L'.' && c != L':' && c != L'-') {
            return {};
        }
    }
    return text;
}

void Metadata(Log& g, wdj::JsonObject const& record) {
    std::wstring type = Str(record, L"type");
    if (g.source == Source::kCodex) {
        auto payload = Child(record, L"payload");
        if (type != L"session_meta" || !payload || g.sessionCreated) {
            return;
        }
        // A fork replays its parent's session record after its own; the first stands.
        g.sessionCreated = TimeOf(payload, L"timestamp");
        if (!g.sessionCreated) {
            g.sessionCreated = 1;  // seen, without a time
        }
        g.SetProject(Str(payload, L"cwd"));
        auto source = Child(payload, L"source");
        auto sub = Child(source, L"subagent");
        std::wstring subText = Str(source, L"subagent");
        if (sub || !subText.empty()) {
            g.subagent = true;
            std::wstring id = Str(payload, L"id");
            g.agentId = Str(payload, L"agent_path");
            if (g.agentId.empty()) g.agentId = Str(payload, L"agent_id");
            if (g.agentId.empty()) g.agentId = id;
            auto spawn = Child(sub, L"thread_spawn");
            std::wstring root = Str(payload, L"session_id");
            g.parentId = !root.empty() && root != id ? root : Str(spawn, L"parent_thread_id");
            if (g.parentId.empty()) g.parentId = Str(payload, L"parent_thread_id");
            std::ranges::transform(g.parentId, g.parentId.begin(), towlower);
            // Role, then nickname; an automatic review names only `other`.
            for (auto const& role : {Str(spawn, L"agent_role"), Str(payload, L"agent_role"),
                                     Str(payload, L"agent_nickname"), Str(spawn, L"agent_nickname"),
                                     Str(sub, L"other"), subText}) {
                if (auto label = Label(role); !label.empty()) {
                    g.role = label;
                    break;
                }
            }
        }
        return;
    }
    if (True(record, L"isSidechain") && !g.subagent) {
        return;
    }
    g.SetProject(Str(record, L"cwd"));
    if (g.subagent) {
        if (auto id = Str(record, L"agentId"); !id.empty()) g.agentId = id;
        return;  // no titles
    }
    if (type == L"custom-title") {
        if (auto t = CleanTitle(Str(record, L"customTitle")); !t.empty()) g.customTitle = t;
    } else if (type == L"ai-title") {
        if (auto t = CleanTitle(Str(record, L"aiTitle")); !t.empty()) g.aiTitle = t;
    } else if (type == L"summary") {
        if (auto t = CleanTitle(Str(record, L"summary")); !t.empty()) g.summary = t;
    }
}

void Codex(Log& g, wdj::JsonObject const& record, int64_t at) {
    std::wstring type = Str(record, L"type");
    auto payload = Child(record, L"payload");
    bool ownSnapshot = g.ownTurnSeen && !g.ignoringInherited;
    if (type == L"token_usage_record") {
        auto delta = Count(Child(payload, L"usage"), L"output_tokens");
        if (g.ignoringInherited || !delta) {
            return;
        }
        std::wstring id = Str(payload, L"turn_id");
        std::wstring current = !g.turnId.empty() ? g.turnId : g.metadataTurnId;
        if (!id.empty() && !current.empty() && id != current) {
            return;
        }
        if (auto response = Str(payload, L"response_id");
            !response.empty() && !g.responses.Insert(response)) {
            return;
        }
        g.usageRecords = true;
        if (!id.empty() && id == current) {
            if (auto total = Count(Child(payload, L"turn_token_usage"), L"output_tokens")) {
                g.turnUsage = {id, *total};
            }
        }
        g.RecordOutput(*delta, at);
        if (g.startedAt) {
            g.output = Add(g.output, *delta);
            g.hasUsage = true;
        }
        return;
    }
    if (type == L"turn_context") {
        std::wstring model = Str(payload, L"model");
        if (model.empty() || g.ignoringInherited) {
            return;
        }
        g.model = model;
        std::wstring effort = Label(Str(payload, L"effort"));
        if (effort.empty()) {
            effort = Label(Str(Child(Child(payload, L"collaboration_mode"), L"settings"),
                               L"reasoning_effort"));
        }
        if (!effort.empty()) {
            g.effort = effort;
        }
        g.SetProject(Str(payload, L"cwd"));
        // A turn whose start fell before the tail is known by its context.
        std::wstring id = Str(payload, L"turn_id");
        if (!id.empty() && !g.TurnOpen() && g.closedTurns.ids.count(id) == 0) {
            g.metadataOpen = true;
            g.metadataTurnId = id;
            g.activityStartedAt = at;
            g.observed = State::kWorking;
            g.ownTurnSeen = true;
            if (at) {
                g.lastActivity = std::max(g.lastActivity, at);
            }
        }
        return;
    }
    if (type == L"response_item" && !g.ignoringInherited && (g.startedAt || g.metadataOpen)) {
        std::wstring item = Str(payload, L"type");
        std::optional<State> tool =
            g.pending.empty() ? std::nullopt : std::optional<State>(State::kTool);
        std::optional<State> phase;
        if (item == L"function_call" || item == L"custom_tool_call") {
            if (auto id = Str(payload, L"call_id"); !id.empty()) {
                g.AddTool(id, Str(payload, L"name"));
            }
            phase = State::kTool;
        } else if (item == L"function_call_output" || item == L"custom_tool_call_output") {
            g.RemoveTool(Str(payload, L"call_id"));
            phase = g.pending.empty() ? State::kWorking : State::kTool;
        } else if (item == L"reasoning") {
            phase = tool.value_or(State::kWorking);
        } else if (item == L"agent_message" ||
                   (item == L"message" && Str(payload, L"role") == L"assistant")) {
            phase = tool.value_or(State::kOutput);
        }
        if (phase) {
            g.observed = *phase;
            if (at) {
                g.lastActivity = std::max(g.lastActivity, at);
            }
        }
        return;
    }
    if (type != L"event_msg") {
        return;
    }
    std::wstring event = Str(payload, L"type");
    if (event == L"task_started") {
        std::wstring id = Str(payload, L"turn_id");
        int64_t start = TimeOf(payload, L"started_at");
        if (!start) {
            start = at;
        }
        // A parent's turn, replayed into a fork with the fork's write time.
        if (start && g.sessionCreated > 1 && start < g.sessionCreated - 1) {
            if (!g.startedAt) {
                g.ignoringInherited = true;
                g.ignoredTurn = id;
                g.model.clear();
                g.effort.clear();
                g.cumulative.reset();
            }
            return;
        }
        if (!id.empty() && (g.closedTurns.ids.count(id) || id == g.turnId)) {
            return;
        }
        g.ignoringInherited = false;
        g.ignoredTurn.clear();
        g.Begin(id, start);
        g.ownTurnSeen = true;
        g.metadataOpen = true;
        g.metadataTurnId = id;
        if (at) {
            g.lastActivity = at;
        }
    } else if (event == L"token_count") {
        auto info = Child(payload, L"info");
        if (ownSnapshot && at && info && at >= g.contextAt) {
            // input_tokens already holds the cached input; the window comes with it.
            if (auto input = Count(Child(info, L"last_token_usage"), L"input_tokens")) {
                g.contextUsed = *input;
                g.contextWindow = Count(info, L"model_context_window").value_or(0);
                g.contextAt = at;
            }
        }
        if (g.ignoringInherited || g.usageRecords || !info) {
            return;
        }
        auto count = Count(Child(info, L"total_token_usage"), L"output_tokens");
        if (!count) {
            if (g.startedAt) {
                g.accurate = false;
            }
            return;
        }
        auto last = Count(Child(info, L"last_token_usage"), L"output_tokens");
        std::optional<int> delta =
            g.cumulative ? (*count >= *g.cumulative ? std::optional<int>(*count - *g.cumulative) : last)
                         : last;
        g.cumulative = *count;
        if (delta && *delta >= 0) {
            g.RecordOutput(*delta, at);
            if (g.startedAt) {
                g.output = Add(g.output, *delta);
                g.hasUsage = true;
            }
        } else if (g.startedAt) {
            g.accurate = false;
        }
    } else if (event == L"task_complete") {
        std::wstring id = Str(payload, L"turn_id");
        if (g.ignoringInherited && id == g.ignoredTurn) {
            return;
        }
        if (!((g.metadataOpen && id == g.metadataTurnId) || (g.startedAt && g.turnId == id))) {
            return;
        }
        g.CloseTurn(State::kComplete, at);
        if (!id.empty()) {
            g.closedTurns.Insert(id);
        }
    } else if (event == L"turn_aborted" || event == L"task_aborted") {
        if (g.ignoringInherited) {
            return;
        }
        std::wstring id = Str(payload, L"turn_id");
        std::wstring current = !g.turnId.empty() ? g.turnId : g.metadataTurnId;
        if (!id.empty() && !current.empty() && id != current) {
            return;
        }
        g.CloseTurn(State::kInterrupted, at);
    }
}

enum class Input { kHuman, kInterrupted, kToolResult, kNone };

// What a Claude user record is: a prompt that starts a turn, an interruption,
// a tool's result, or neither (a slash command's echo).
Input ClaudeInput(wdj::JsonObject const& record, wdj::JsonObject const& message) {
    std::wstring text = Str(message, L"content");
    auto blocks = Objects(message, L"content");
    if (text.empty()) {
        for (auto const& block : blocks) {
            if (Str(block, L"type") == L"text") {
                text = Str(block, L"text");
                break;
            }
        }
    }
    if (text.starts_with(L"[Request interrupted")) {
        return Input::kInterrupted;
    }
    if (std::ranges::any_of(blocks, [](auto const& b) { return Str(b, L"type") == L"tool_result"; })) {
        return Input::kToolResult;
    }
    if (auto origin = Child(record, L"origin")) {
        return Str(origin, L"kind").empty() ? Input::kNone : Input::kHuman;
    }
    if (text.starts_with(L"<command-name>") || text.starts_with(L"<command-message>") ||
        text.starts_with(L"<local-command-")) {
        return Input::kNone;
    }
    return !text.empty() || !blocks.empty() ? Input::kHuman : Input::kNone;
}

void Claude(Log& g, wdj::JsonObject const& record, int64_t at, int64_t previousLog) {
    if (True(record, L"isSidechain") && !g.subagent) {
        return;
    }
    // Restored or branched sessions write earlier records again.
    if (auto uuid = Str(record, L"uuid"); !uuid.empty() && !g.records.Insert(uuid)) {
        return;
    }
    if (at && previousLog && at < previousLog - 600) {
        return;
    }
    if (auto id = Str(record, L"sessionId"); !id.empty()) {
        g.sessionId = id;
    }
    std::wstring type = Str(record, L"type");
    auto message = Child(record, L"message");
    if (type == L"user") {
        bool prompt = (!True(record, L"isMeta") || Child(record, L"origin")) &&
                      !True(record, L"isCompactSummary");
        if (!prompt) {
            return;
        }
        switch (ClaudeInput(record, message)) {
            case Input::kHuman:
                g.Begin(Str(record, L"uuid"), at);
                if (at) {
                    g.lastActivity = std::max(g.lastActivity, at);
                }
                break;
            case Input::kInterrupted:
                g.CloseTurn(State::kInterrupted, at);
                break;
            case Input::kToolResult:
                g.softClosed = false;
                g.metadataOpen = true;
                for (auto const& block : Objects(message, L"content")) {
                    if (Str(block, L"type") == L"tool_result") {
                        g.RemoveTool(Str(block, L"tool_use_id"));
                    }
                }
                g.observed = g.pending.empty() ? State::kWorking : State::kTool;
                if (at) {
                    g.lastActivity = std::max(g.lastActivity, at);
                }
                if (True(record, L"toolEndsTurn")) {
                    g.CloseTurn(State::kComplete, at);
                }
                break;
            default:
                break;
        }
    } else if (type == L"assistant") {
        auto usage = Child(message, L"usage");
        auto count = Count(usage, L"output_tokens");
        std::wstring id = Str(message, L"id");
        if (!count || id.empty() || g.previousMessages.ids.count(id)) {
            return;
        }
        std::wstring model = Str(message, L"model");
        bool errored = True(record, L"isApiErrorMessage") || model == L"<synthetic>";
        if (!errored) {
            if (!model.empty()) {
                g.model = model;
            }
            // Context held by this request: every input-side count. Claude logs
            // no window size.
            if (auto input = Count(usage, L"input_tokens"); input && at && at >= g.contextAt) {
                g.contextUsed = Add(Add(*input, Count(usage, L"cache_creation_input_tokens").value_or(0)),
                                    Count(usage, L"cache_read_input_tokens").value_or(0));
                g.contextWindow = 0;
                g.contextAt = at;
            }
        }
        int prior = g.messages.contains(id) ? g.messages[id] : 0;
        g.messages[id] = std::max(prior, *count);
        if (at) {
            g.lastActivity = std::max(g.lastActivity, at);
        }
        g.softClosed = false;
        auto blocks = Objects(message, L"content");
        auto has = [&](PCWSTR kind) {
            return std::ranges::any_of(blocks, [&](auto const& b) { return Str(b, L"type") == kind; });
        };
        bool usesTool = has(L"tool_use");
        if (usesTool) {
            for (auto const& block : blocks) {
                if (Str(block, L"type") == L"tool_use") {
                    if (auto toolId = Str(block, L"id"); !toolId.empty()) {
                        g.AddTool(toolId, Str(block, L"name"));
                    }
                }
            }
            g.observed = State::kTool;
        } else if (has(L"text")) {
            g.observed = g.pending.empty() ? State::kOutput : State::kTool;
        } else if (has(L"thinking")) {
            g.observed = g.pending.empty() ? State::kWorking : State::kTool;
        }
        if (*count > prior) {
            if (g.startedAt) {
                g.output = Add(g.output, *count - prior);
                g.hasUsage = true;
            }
            g.RecordOutput(*count - prior, at);
        }
        if (errored) {
            g.softClosed = true;
            g.observed = State::kInterrupted;
        } else if (!usesTool && g.pending.empty()) {
            std::wstring stop = Str(message, L"stop_reason");
            if (stop == L"end_turn" || stop == L"stop_sequence") {
                g.softClosed = true;
                g.observed = State::kComplete;
                if (g.startedAt) {
                    g.RecordCompletion();
                }
            }
        }
    } else if (type == L"system") {
        if (at && g.startedAt && at < g.startedAt) {
            return;  // an earlier turn's marker
        }
        std::wstring subtype = Str(record, L"subtype");
        if (subtype == L"turn_duration" || subtype == L"stop_hook_summary") {
            g.CloseTurn(State::kComplete, subtype == L"turn_duration" ? at : 0);
        } else if (subtype == L"turn_aborted" || subtype == L"task_aborted" ||
                   subtype == L"interrupted") {
            g.CloseTurn(State::kInterrupted, at);
        }
    }
}

std::wstring Widen(std::string_view bytes) {
    int n = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(), nullptr, 0);
    std::wstring text(n > 0 ? n : 0, L'\0');
    if (n > 0) {
        MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(), text.data(), n);
    }
    return text;
}

// A line too long to read whole still ends a tool call; its id is near the start.
void Oversized(Log& g, std::string_view line) {
    std::string_view head = line.substr(0, kPrefixBytes);
    auto value = [&](std::string_view key) -> std::string {
        std::string needle = "\"" + std::string(key) + "\":\"";
        size_t at = head.find(needle);
        if (at == std::string_view::npos) {
            return {};
        }
        size_t start = at + needle.size(), end = head.find('"', start);
        return end == std::string_view::npos ? std::string() : std::string(head.substr(start, end - start));
    };
    std::string id;
    if (g.source == Source::kCodex) {
        if (g.ignoringInherited || head.find("\"type\":\"response_item\"") == std::string_view::npos ||
            (head.find("\"type\":\"function_call_output\"") == std::string_view::npos &&
             head.find("\"type\":\"custom_tool_call_output\"") == std::string_view::npos)) {
            return;
        }
        id = value("call_id");
    } else {
        if (head.find("\"type\":\"user\"") == std::string_view::npos ||
            head.find("\"type\":\"tool_result\"") == std::string_view::npos ||
            (!g.subagent && head.find("\"isSidechain\":true") != std::string_view::npos)) {
            return;
        }
        id = value("tool_use_id");
    }
    if (!id.empty() && g.RemoveTool(Widen(id)) && g.TurnOpen()) {
        g.observed = g.pending.empty() ? State::kWorking : State::kTool;
    }
}

void Line(Log& g, std::string_view line, bool metadataOnly) {
    if (line.empty()) {
        return;
    }
    if (line.size() > kMaxLine) {
        if (!metadataOnly) {
            Oversized(g, line);
        }
        return;
    }
    wdj::JsonObject record{nullptr};
    if (!wdj::JsonObject::TryParse(Widen(line), record)) {
        return;
    }
    Metadata(g, record);
    if (metadataOnly) {
        return;
    }
    int64_t at = TimeOf(record, L"timestamp");
    int64_t previous = g.lastLogAt;
    if (g.source == Source::kCodex) {
        Codex(g, record, at);
    } else {
        Claude(g, record, at, previous);
    }
    if (at) {
        g.lastLogAt = std::max(g.lastLogAt, at);
    }
}

// ---- files (worker) -------------------------------------------------------------------

bool ReadAt(HANDLE file, uint64_t at, size_t length, std::string& out) {
    out.resize(length);
    OVERLAPPED where{};
    where.Offset = (DWORD)at;
    where.OffsetHigh = (DWORD)(at >> 32);
    DWORD got = 0;
    if (length && !ReadFile(file, out.data(), (DWORD)length, &got, &where)) {
        return false;
    }
    out.resize(got);
    return true;
}

// Reads what was added to the log since the last look; a replaced or shorter
// file starts over. The first look reads a Codex log's head for its own session
// record, then only the tail.
void Read(Log& g, uint64_t size, int64_t created) {
    if (g.started && (created != g.created || size < g.offset)) {
        Log fresh;
        fresh.source = g.source;
        fresh.path = g.path;
        fresh.sessionId = g.sessionId;  // from the file name
        g = std::move(fresh);
    }
    if (g.started && size == g.offset) {
        return;
    }
    winrt::file_handle file{CreateFileW(g.path.c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (!file) {
        return;
    }
    std::string bytes;
    bool cut = false;  // the tail starts inside a line
    if (!g.started) {
        g.started = true;
        g.created = created;
        if (g.source == Source::kCodex && ReadAt(file.get(), 0, (size_t)std::min(size, kHeadBytes), bytes)) {
            size_t end = bytes.size() == size ? bytes.size() : bytes.rfind('\n');
            std::string_view head(bytes.data(), end == std::string::npos ? 0 : end);
            for (size_t start = 0; start < head.size();) {
                size_t next = head.find('\n', start);
                if (next == std::string_view::npos) next = head.size();
                Line(g, head.substr(start, next - start), true);
                start = next + 1;
            }
        }
        uint64_t tail = g.subagent ? kAgentTailBytes : kTailBytes;
        g.offset = size > tail ? size - tail : 0;
        cut = g.offset > 0;
    }
    uint64_t from = g.offset;
    size_t length = (size_t)std::min(size - from, kReadBudget);
    if (!ReadAt(file.get(), from, length, bytes)) {
        return;
    }
    g.offset = from + bytes.size();
    std::string data = std::move(g.partial);
    g.partial.clear();
    data += bytes;
    size_t start = 0;
    if (cut) {
        size_t first = data.find('\n');
        start = first == std::string::npos ? data.size() : first + 1;
    }
    std::string_view view(data);
    while (start < view.size()) {
        size_t next = view.find('\n', start);
        if (next == std::string_view::npos) {
            if (view.size() - start <= kMaxLine) {
                g.partial = std::string(view.substr(start));  // waits for its newline
            }
            break;
        }
        std::string_view line = view.substr(start, next - start);
        if (!line.empty() && line.back() == '\r') {
            line.remove_suffix(1);
        }
        Line(g, line, false);
        start = next + 1;
    }
}

struct Found {
    Source source = Source::kClaude;
    std::wstring path;
    int64_t written = 0;
    uint64_t size = 0;
    int64_t created = 0;
    bool subagent = false;
};

void FilesIn(Source source, std::wstring const& dir, PCWSTR pattern, std::vector<Found>& out,
             bool (*skip)(std::wstring_view) = nullptr) {
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((dir + L"\\" + pattern).c_str(), FindExInfoBasic, &data,
                                   FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY || (skip && skip(data.cFileName))) {
            continue;
        }
        out.push_back({source, dir + L"\\" + data.cFileName, Unix(data.ftLastWriteTime),
                       (uint64_t)data.nFileSizeHigh << 32 | data.nFileSizeLow,
                       Unix(data.ftCreationTime)});
    } while (FindNextFileW(find, &data));
    FindClose(find);
}

std::vector<std::wstring> Dirs(std::wstring const& dir) {
    std::vector<std::wstring> out;
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &data,
                                   FindExSearchLimitToDirectories, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return out;
    }
    do {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && wcscmp(data.cFileName, L".") &&
            wcscmp(data.cFileName, L"..") && !(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            out.push_back(dir + L"\\" + data.cFileName);
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return out;
}

std::wstring Env(PCWSTR name) {
    WCHAR value[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(name, value, ARRAYSIZE(value));
    return n && n < ARRAYSIZE(value) ? std::wstring(value) : std::wstring();
}

std::wstring Home() {
    return Env(L"USERPROFILE");
}

// Claude Code's project folders: CLAUDE_CONFIG_DIR's (a comma list), the XDG
// one, and ~\.claude's.
std::vector<std::wstring> ClaudeRoots() {
    std::vector<std::wstring> roots;
    auto add = [&](std::wstring dir) {
        while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/' || dir.back() == L' ')) {
            dir.pop_back();
        }
        if (dir.empty() || !usage::LocalPath(dir)) {
            return;
        }
        dir += L"\\projects";
        if (std::ranges::none_of(roots, [&](auto const& r) { return _wcsicmp(r.c_str(), dir.c_str()) == 0; })) {
            roots.push_back(dir);
        }
    };
    std::wstring list = Env(L"CLAUDE_CONFIG_DIR");
    for (size_t start = 0; start <= list.size();) {
        size_t comma = list.find(L',', start);
        if (comma == std::wstring::npos) comma = list.size();
        size_t first = list.find_first_not_of(L' ', start);
        if (first != std::wstring::npos && first < comma) add(list.substr(first, comma - first));
        start = comma + 1;
    }
    std::wstring xdg = Env(L"XDG_CONFIG_HOME");
    std::wstring home = Home();
    if (!xdg.empty()) {
        add(xdg + L"\\claude");
    } else if (!home.empty()) {
        add(home + L"\\.config\\claude");
    }
    if (!home.empty()) {
        add(home + L"\\.claude");
    }
    return roots;
}

std::wstring CodexHome() {
    std::wstring dir = Env(L"CODEX_HOME");
    if (dir.empty() || !usage::LocalPath(dir)) {
        std::wstring home = Home();
        dir = home.empty() ? L"" : home + L"\\.codex";
    }
    return dir;
}

// The newest `count` of `found` written in the last day.
std::vector<Found> Newest(std::vector<Found> found, int64_t now) {
    std::erase_if(found, [&](Found const& f) { return now - f.written > kRecentSeconds; });
    std::ranges::sort(found, std::greater{}, &Found::written);
    if (found.size() > kPerSource) {
        found.resize(kPerSource);
    }
    return found;
}

// A session's subagents: agent-*.jsonl in <session>\subagents and up to two
// folders under it (a workflow's agents).
void AgentsIn(std::wstring const& dir, int depth, std::vector<Found>& out) {
    FilesIn(Source::kClaude, dir, L"agent-*.jsonl", out);
    if (depth > 0) {
        for (auto const& child : Dirs(dir)) {
            AgentsIn(child, depth - 1, out);
        }
    }
}

std::vector<Found> FindClaude(int64_t now) {
    std::vector<Found> found;
    for (auto const& root : ClaudeRoots()) {
        for (auto const& project : Dirs(root)) {
            FilesIn(Source::kClaude, project, L"*.jsonl", found, [](std::wstring_view name) {
                return name.find(L".orphaned-") != std::wstring_view::npos;
            });
        }
    }
    found = Newest(std::move(found), now);
    // The subagents of these sessions only, the newest of each, then of all.
    std::vector<Found> agents;
    for (auto const& f : found) {
        std::vector<Found> own;
        AgentsIn(f.path.substr(0, f.path.size() - 6) + L"\\subagents", 2, own);
        std::erase_if(own, [&](Found const& a) { return now - a.written > kAgentSeconds; });
        std::ranges::sort(own, std::greater{}, &Found::written);
        if (own.size() > kAgentsPerSession) {
            own.resize(kAgentsPerSession);
        }
        agents.insert(agents.end(), own.begin(), own.end());
    }
    std::ranges::sort(agents, std::greater{}, &Found::written);
    if (agents.size() > kMaxAgents) {
        agents.resize(kMaxAgents);
    }
    for (auto& a : agents) {
        a.subagent = true;
        found.push_back(std::move(a));
    }
    return found;
}

// sessions\YYYY\MM\DD\rollout-*.jsonl; a session started days ago may still run,
// so every day's folder is listed, newest first, until a day has nothing recent.
std::vector<Found> FindCodex(int64_t now) {
    std::vector<Found> found;
    std::wstring home = CodexHome();
    if (home.empty()) {
        return found;
    }
    auto desc = [](std::vector<std::wstring> v) {
        std::ranges::sort(v, std::greater{});
        return v;
    };
    for (auto const& year : desc(Dirs(home + L"\\sessions"))) {
        for (auto const& month : desc(Dirs(year))) {
            for (auto const& day : desc(Dirs(month))) {
                FilesIn(Source::kCodex, day, L"*.jsonl", found);
            }
        }
    }
    return Newest(std::move(found), now);
}

// The trailing UUID of rollout-<time>-<uuid>.jsonl, which names the thread.
std::wstring CodexId(std::wstring const& path) {
    size_t dot = path.rfind(L".jsonl");
    if (dot == std::wstring::npos || dot < 36) {
        return {};
    }
    std::wstring id = path.substr(dot - 36, 36);
    std::ranges::transform(id, id.begin(), towlower);
    return id;
}

// Codex keeps its thread names in session_index.jsonl ({id, thread_name,
// updated_at}); the newest name of an id wins. Its tail is read again as it or
// the logs shown change, and only the names of those logs are kept.
struct Names {
    std::wstring path;
    uint64_t size = 0;
    int64_t written = 0;
    std::vector<std::wstring> ids;  // the logs' sessions, lower case
    std::unordered_map<std::wstring, std::pair<int64_t, std::wstring>> byId;

    void Update(std::vector<std::wstring> wanted) {
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (path.empty() || !GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data)) {
            return;
        }
        uint64_t now = (uint64_t)data.nFileSizeHigh << 32 | data.nFileSizeLow;
        int64_t at = Unix(data.ftLastWriteTime);
        std::ranges::sort(wanted);
        if (now == size && at == written && wanted == ids) {
            return;
        }
        ids = std::move(wanted);
        byId.clear();
        size = now;
        written = at;
        winrt::file_handle file{CreateFileW(path.c_str(), GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
        std::string bytes;
        uint64_t from = size > kReadBudget ? size - kReadBudget : 0;
        if (!file || !ReadAt(file.get(), from, (size_t)(size - from), bytes)) {
            return;
        }
        std::string_view view(bytes);
        size_t start = 0;
        if (from > 0) {  // the tail starts inside a line
            size_t first = view.find('\n');
            start = first == std::string_view::npos ? view.size() : first + 1;
        }
        while (start < view.size()) {
            size_t next = view.find('\n', start);
            if (next == std::string_view::npos) break;
            std::string_view line = view.substr(start, next - start);
            start = next + 1;
            wdj::JsonObject record{nullptr};
            if (line.size() > 4096 || !wdj::JsonObject::TryParse(Widen(line), record)) continue;
            std::wstring id = Str(record, L"id"), name = CleanTitle(Str(record, L"thread_name"));
            if (id.empty() || name.empty()) continue;
            std::ranges::transform(id, id.begin(), towlower);
            if (!std::ranges::binary_search(ids, id)) continue;
            int64_t updated = TimeOf(record, L"updated_at");
            auto& entry = byId[id];
            if (updated >= entry.first) entry = {updated, name};
        }
    }
};

// Claude writes a subagent's kind in <log>.meta.json beside it; only its
// "agentType" is looked for (the file also holds the task, never read).
std::wstring AgentType(std::wstring const& log) {
    std::wstring path = log.substr(0, log.size() - 6) + L".meta.json";
    winrt::file_handle file{CreateFileW(path.c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    LARGE_INTEGER size;
    std::string bytes;
    if (!file || !GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 16 * 1024 || !ReadAt(file.get(), 0, (size_t)size.QuadPart, bytes)) {
        return {};
    }
    constexpr std::string_view kNeedle = "\"agentType\"";
    size_t at = bytes.find(kNeedle);
    if (at == std::string::npos) {
        return {};
    }
    at = bytes.find_first_not_of(" \t\r\n:", at + kNeedle.size());
    if (at == std::string::npos || bytes[at] != '"') {
        return {};
    }
    size_t end = bytes.find('"', at + 1);
    return end == std::string::npos ? std::wstring() : Label(Widen(bytes.substr(at + 1, end - at - 1)));
}

// ---- the rows (worker, then the taskbar thread) ----------------------------------------

struct Row {
    Source source = Source::kClaude;
    State state = State::kIdle;
    std::wstring tool;  // the running tool's name
    std::wstring title, project, model, effort;
    std::wstring sessionId, projectPath;  // for its details
    bool subagent = false;
    std::wstring agentId, role, parentId;
    std::vector<Row> agents;  // a session's subagents, live first
    int64_t liveAt = 0, turnStartedAt = 0, lastOutputAt = 0;
    int lastOutputDelta = 0, lastTurnOutput = 0;
    std::optional<int> turnOutput;
    int contextUsed = -1, contextWindow = 0;

    bool Live() const {
        return state == State::kInput || state == State::kTool || state == State::kWorking ||
               state == State::kOutput || state == State::kStale;
    }
};

struct Snapshot {
    std::vector<Row> rows;
    int64_t at = 0;
};

std::mutex g_lock;  // held by the one look at a time
[[clang::no_destroy]] std::unordered_map<std::wstring, Log> g_logs;
[[clang::no_destroy]] Names g_names;
int g_looks = 0;
std::atomic<bool> g_findNow{true};  // the next look searches for logs first
std::vector<Found> g_found;
std::atomic<bool> g_looking;
std::atomic<bool> g_wanted;  // the details are open
[[clang::no_destroy]] std::shared_ptr<const Snapshot> g_snapshot;  // taskbar thread
void (*g_onRows)() = nullptr;  // taskbar thread

Row RowOf(Log const& g, int64_t now) {
    Row r;
    r.source = g.source;
    r.state = g.Activity(now);
    r.tool = r.state == State::kTool || r.state == State::kInput ? g.RunningTool() : L"";
    if (g.source == Source::kCodex) {
        auto it = g_names.byId.find(g.sessionId);
        r.title = it != g_names.byId.end() ? it->second.second : std::wstring();
    } else {
        r.title = g.Title();
    }
    r.project = g.project;
    r.projectPath = g.projectPath;
    r.sessionId = g.sessionId;
    r.subagent = g.subagent;
    r.agentId = g.agentId;
    r.role = g.role;
    r.parentId = g.subagent && g.source == Source::kClaude ? g.sessionId : g.parentId;
    r.model = g.model;
    r.effort = g.effort;
    r.liveAt = g.LiveAt();
    r.turnStartedAt = g.TurnOpen() ? g.activityStartedAt : 0;
    r.lastOutputAt = g.lastOutputAt;
    r.lastOutputDelta = g.lastOutputDelta;
    r.lastTurnOutput = g.lastTurnOutput;
    r.turnOutput = g.TurnOutput();
    r.contextUsed = g.contextUsed;
    r.contextWindow = g.contextWindow;
    return r;
}

void Look() {
    std::lock_guard guard(g_lock);
    int64_t now = usage::Now();
    if (g_findNow.exchange(false) || ++g_looks % kFindEvery == 0) {
        g_found = FindClaude(now);
        std::vector<Found> codex = FindCodex(now);
        g_found.insert(g_found.end(), codex.begin(), codex.end());
        std::wstring home = CodexHome();
        g_names.path = home.empty() ? L"" : home + L"\\session_index.jsonl";
        // Logs no longer among the newest are forgotten.
        std::erase_if(g_logs, [](auto const& entry) {
            return std::ranges::none_of(g_found, [&](Found const& f) { return f.path == entry.first; });
        });
    }
    if (!g_wanted || core::g_unloading) {
        return;
    }
    auto snapshot = std::make_shared<Snapshot>();
    bool anyCodex = false;
    for (auto& f : g_found) {
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (!GetFileAttributesExW(f.path.c_str(), GetFileExInfoStandard, &data)) {
            continue;
        }
        uint64_t size = (uint64_t)data.nFileSizeHigh << 32 | data.nFileSizeLow;
        bool codex = f.source == Source::kCodex;
        auto [it, added] = g_logs.try_emplace(f.path);
        Log& g = it->second;
        if (added) {
            g.source = codex ? Source::kCodex : Source::kClaude;
            g.path = f.path;
            g.subagent = f.subagent;
            if (codex) {
                g.sessionId = CodexId(f.path);
            }
        }
        anyCodex |= codex;
        Read(g, size, Unix(data.ftCreationTime));
        if (g.subagent && !codex && g.role.empty() && g.roleLooks < 3) {
            g.roleLooks++;
            g.role = AgentType(f.path);
        }
        if (!g_wanted || core::g_unloading) {
            return;
        }
    }
    if (anyCodex) {
        std::vector<std::wstring> ids;
        for (auto const& [path, g] : g_logs) {
            if (g.source == Source::kCodex && !g.sessionId.empty()) ids.push_back(g.sessionId);
        }
        g_names.Update(std::move(ids));
    }
    now = usage::Now();
    std::vector<Row> agents;
    for (auto const& [path, g] : g_logs) {
        if (g.lastLogAt || g.lastActivity) {
            (g.subagent ? agents : snapshot->rows).push_back(RowOf(g, now));
        }
    }
    // Live ones first, then by their last sign of life.
    auto order = [](Row const& a, Row const& b) {
        if (a.Live() != b.Live()) return a.Live();
        return a.liveAt > b.liveAt;
    };
    std::ranges::sort(agents, order);
    for (auto& a : agents) {
        auto parent = std::ranges::find_if(snapshot->rows, [&](Row const& r) {
            return r.source == a.source && !a.parentId.empty() &&
                   _wcsicmp(r.sessionId.c_str(), a.parentId.c_str()) == 0;
        });
        if (parent != snapshot->rows.end()) {
            parent->agents.push_back(std::move(a));
        } else if (a.source == Source::kCodex) {
            snapshot->rows.push_back(std::move(a));  // its session isn't among those read
        }
    }
    std::ranges::sort(snapshot->rows, order);
    snapshot->at = now;
    core::Post([snapshot] {
        if (!g_wanted) {
            return;  // closed meanwhile
        }
        g_snapshot = snapshot;
        if (g_onRows) {
            g_onRows();
        }
    });
}

// The details closed: nothing read for them stays. A look running now holds
// the lock, so the job lets go after it, unless they opened again meanwhile.
void Forget() {
    g_snapshot = nullptr;
    folders::StartJob([] {
        std::lock_guard guard(g_lock);
        if (g_wanted) {
            return;
        }
        std::unordered_map<std::wstring, Log>().swap(g_logs);
        g_names = Names{};
        std::vector<Found>().swap(g_found);
        g_looks = 0;
        g_findNow = true;
    });
}

// Looks again unless a look is running; the worker holds a reference to the
// module, so an unload waits for it to return.
void Request() {
    if (!g_wanted || core::g_unloading || g_looking.exchange(true)) {
        return;
    }
    folders::StartJob([] {
        try {
            Look();
        } catch (...) {
            Wh_Log(L"Session look failed: %08X", winrt::to_hresult());
        }
        g_looking = false;
    });
}

}  // namespace ai_sessions

// ============================================================================
namespace usage_detail {
// ============================================================================
// A click on the usage meter opens its details: a card per account as Bloom's
// home draws them, with every limit's number, bar and time to its reset, then
// the Claude Code and Codex sessions. Drawn in Bloom's design system (its
// palette, accent, popup surface, type), on the folder popup's flyout, which
// holds the taskbar out and closes on a press elsewhere. Everything is made as
// it opens and let go as it closes; nothing is read while it is closed. The
// numbers count used or left as the meter does, and the switch at the top turns
// them as a right-click on the meter does. Taskbar thread only.

namespace wux = winrt::Windows::UI::Xaml;
using winrt::Windows::UI::Color;

// ---- Bloom's design system -----------------------------------------------------------
// Bloom's Ui\Theme\Palette.cs tokens, dark and light; the accent is Bloom's pick.

struct Colors {
    uint32_t text, dim, faint, card, tile, track, line, popup, popupLine, edge, hilite, hover, warn,
        danger;
    uint32_t accent = 0, accentSoft = 0, accentText = 0;
};
constexpr Colors kDark{0xFFECEDEE, 0xFFA1A1AA, 0xFF97989A, 0x0DFFFFFF, 0x14FFFFFF,
                       0x1AFFFFFF, 0x14FFFFFF, 0xF527272A, 0x14FFFFFF, 0x26FFFFFF,
                       0x14FFFFFF, 0x14FFFFFF, 0xFFF7B750, 0xFFE77578};
constexpr Colors kLight{0xFF11181C, 0xFF3F3F46, 0xFF52525B, 0x0D111111, 0x14111111,
                        0x1A111111, 0x14111111, 0xFAFAFAFA, 0x14111111, 0x26111111,
                        0x99FAFAFA, 0x14111111, 0xFF62420E, 0xFFB3363A};
// Theme.Sans: Windows' UI font, Malgun Gothic for Hangul.
constexpr PCWSTR kFont = L"Segoe UI, Segoe UI Emoji, Segoe UI Symbol, Malgun Gothic";
constexpr PCWSTR kIconFont = L"Segoe Fluent Icons, Segoe MDL2 Assets";
constexpr double kPopupRadius = 12, kCardRadius = 12, kSmoothing = 0.6;
constexpr double kShadowBlur = 26, kShadowDepth = 6, kShadowOpacity = 0.3;  // LiftMenu

// Bloom's Hsl (Palette.cs).
void ToHsl(uint32_t argb, double& h, double& s, double& l) {
    double r = BYTE(argb >> 16) / 255.0, g = BYTE(argb >> 8) / 255.0, b = BYTE(argb) / 255.0;
    double hi = std::max({r, g, b}), lo = std::min({r, g, b}), d = hi - lo;
    l = (hi + lo) / 2;
    if (d < 1e-9) {
        h = s = 0;
        return;
    }
    s = l > 0.5 ? d / (2 - hi - lo) : d / (hi + lo);
    h = 60 * (hi == r ? (g - b) / d + (g < b ? 6 : 0) : hi == g ? (b - r) / d + 2 : (r - g) / d + 4);
}

uint32_t FromHsl(BYTE a, double h, double s, double l) {
    double q = l < 0.5 ? l * (1 + s) : l + s - l * s, p = 2 * l - q, hk = h / 360;
    auto channel = [&](double t) {
        if (t < 0) t += 1;
        if (t > 1) t -= 1;
        return t < 1.0 / 6 ? p + (q - p) * 6 * t
               : t < 0.5   ? q
               : t < 2.0 / 3 ? p + (q - p) * (2.0 / 3 - t) * 6
                             : p;
    };
    auto byte = [](double v) { return (uint32_t)std::lround(std::clamp(v, 0.0, 1.0) * 255); };
    uint32_t r = byte(s < 1e-9 ? l : channel(hk + 1.0 / 3)), g = byte(s < 1e-9 ? l : channel(hk)),
             b = byte(s < 1e-9 ? l : channel(hk - 1.0 / 3));
    return (uint32_t)a << 24 | r << 16 | g << 8 | b;
}

// The accent Bloom shows, dark and light: its "accent" in settings.json (a preset,
// or "custom" with "accentCustom", the light theme's colour). Rose without one.
// Only the two values are looked for, in the file's bytes, once per opening.
std::pair<uint32_t, uint32_t> BloomAccent() {
    std::pair<uint32_t, uint32_t> rose{0xFFFF7A9E, 0xFFE04E78};
    WCHAR base[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"APPDATA", base, ARRAYSIZE(base));
    if (!n || n >= ARRAYSIZE(base)) {
        return rose;
    }
    winrt::file_handle file{CreateFileW((std::wstring(base) + L"\\Bloom\\settings.json").c_str(),
                                        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    LARGE_INTEGER size;
    if (!file || !GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > 4 * 1024 * 1024) {
        return rose;
    }
    std::string bytes((size_t)size.QuadPart, '\0');
    DWORD got = 0;
    if (!ReadFile(file.get(), bytes.data(), (DWORD)bytes.size(), &got, nullptr)) {
        return rose;
    }
    bytes.resize(got);
    auto value = [&](std::string_view key) {
        std::string needle = "\"" + std::string(key) + "\"";
        size_t at = bytes.find(needle);
        if (at == std::string::npos) return std::string();
        at = bytes.find_first_not_of(" \t\r\n:", at + needle.size());
        if (at == std::string::npos || bytes[at] != '"') return std::string();
        size_t end = bytes.find('"', at + 1);
        return end == std::string::npos ? std::string() : bytes.substr(at + 1, end - at - 1);
    };
    std::string key = value("accent");
    static constexpr std::pair<const char*, std::pair<uint32_t, uint32_t>> kPresets[] = {
        {"rose", {0xFFFF7A9E, 0xFFE04E78}},      {"lavender", {0xFFB49CFF, 0xFF7B5BDB}},
        {"sky", {0xFF72B6FF, 0xFF2F72D6}},       {"mint", {0xFF5ED3AE, 0xFF168F6C}},
        {"tangerine", {0xFFFF9B5E, 0xFFD9652A}}, {"lemon", {0xFFEDC94F, 0xFFA87C00}},
        {"gray", {0xFFE4E4E4, 0xFF2E2E2E}},
    };
    for (auto const& [name, pair] : kPresets) {
        if (key == name) return pair;
    }
    if (key == "custom") {
        std::string t = value("accentCustom");
        if (!t.empty() && t[0] == '#') t.erase(0, 1);
        if (t.size() == 6 && t.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
            uint32_t light = 0xFF000000u | (uint32_t)strtoul(t.c_str(), nullptr, 16);
            double h, s, l;
            ToHsl(light, h, s, l);
            return {FromHsl(0xFF, h, s, std::max(l, std::min(0.86, l + 0.18))), light};
        }
    }
    return rose;
}

// The palette with the accent and what Bloom makes of it (Palette.Recolor).
Colors MakeColors(bool dark) {
    Colors c = dark ? kDark : kLight;
    auto [darkAccent, lightAccent] = BloomAccent();
    c.accent = dark ? darkAccent : lightAccent;
    c.accentSoft = 0x1F000000u | (c.accent & 0xFFFFFF);
    double h, s, l;
    ToHsl(c.accent, h, s, l);
    c.accentText = FromHsl(0xFF, h, s, std::clamp(l + (dark ? 0.05 : -0.06), 0.0, 1.0));
    return c;
}

Color ToColor(uint32_t argb) {
    return {BYTE(argb >> 24), BYTE(argb >> 16), BYTE(argb >> 8), BYTE(argb)};
}

Media::SolidColorBrush Brush(uint32_t argb) {
    return Media::SolidColorBrush(ToColor(argb));
}

// ---- the popup's state ---------------------------------------------------------------------

// The popup's folder name: '|' never is in a folder's path.
constexpr PCWSTR kKey = L"|usage";
constexpr double kWidth = 340;
constexpr double kPadding = 14;       // inside the surface
constexpr double kCardPadX = 14, kCardPadY = 12;  // Bloom's home cards
constexpr double kGap = 8;            // between cards
constexpr double kMarkSize = 16;
constexpr double kTitleSize = 14, kNameSize = 12.5, kTextSize = 11.5, kSmallSize = 11;

// What one opening holds; reset as it closes, so nothing stays between openings.
struct Open {
    Colors colors{};
    bool dark = true;
    double rasterization = 1;  // the taskbar's pixels per DIP, for the logos
    // Held, not weak: XAML lets go of a text block's object while only its tree
    // holds it, and a weak reference to it comes back empty.
    Controls::StackPanel body{nullptr};
    Controls::TextBlock sessionsSummary{nullptr};
    Controls::StackPanel sessionsList{nullptr};
    DispatcherTimer tick{nullptr};
    int ticks = 0;
    int renders = 0;  // for the first diagnostics only
    // The list's state, by its elements' tags: the open session's key, what
    // the pointer is on and pressed, and the copy button just used.
    std::wstring expanded, hover, pressed, copied;
    int64_t copiedAt = 0;
};
[[clang::no_destroy]] std::optional<Open> g_open;
// Set when the popup hooked the tray's hide timer itself (the folders, which do
// it otherwise, are off); undone as the meter goes.
bool g_ownsTimerHook = false;

bool Ours() {
    return folders::g_popup && folders::g_popup->model.folder == kKey && g_open;
}

Colors const& C() {
    return g_open->colors;
}

// ---- pieces -------------------------------------------------------------------------------------

Controls::TextBlock Text(std::wstring const& text, double size, uint32_t color,
                         winrt::Windows::UI::Text::FontWeight weight =
                             winrt::Windows::UI::Text::FontWeights::Normal()) {
    Controls::TextBlock t;
    t.Text(text);
    t.FontSize(size);
    t.FontFamily(Media::FontFamily{kFont});
    t.FontWeight(weight);
    t.Foreground(Brush(color));
    t.TextTrimming(TextTrimming::CharacterEllipsis);
    t.VerticalAlignment(VerticalAlignment::Center);
    return t;
}

auto Semi() {
    return winrt::Windows::UI::Text::FontWeights::SemiBold();
}

auto Medium() {
    return winrt::Windows::UI::Text::FontWeights::Medium();
}

Documents::Run Span(std::wstring const& text, double size, uint32_t color, bool medium = false) {
    Documents::Run run;
    run.Text(text);
    run.FontSize(size);
    run.Foreground(Brush(color));
    if (medium) {
        run.FontWeight(Medium());
    }
    return run;
}

// `shape` drawn as `owner`'s smooth-cornered outline, inset by `inset`, following
// its size. It sits on a canvas, which asks for no room, so the outline never
// changes the size it follows: a shape in the layout itself would feed its own
// size back, and a layout that doesn't settle is a crash (XAML's layout cycle).
Controls::Canvas Outline(Controls::Grid const& owner, Shapes::Path const& shape, double radius,
                         double inset) {
    Controls::Canvas holder;
    holder.IsHitTestVisible(false);
    holder.Children().Append(shape);
    if (folders::g_popup) {
        folders::Keep(*folders::g_popup,
                      owner.SizeChanged(winrt::auto_revoke, [shape, radius, inset](
                                                                auto&&, SizeChangedEventArgs const& e) {
                          core::Guard(L"Usage outline", [&] {
                              auto size = e.NewSize();
                              shape.Data(folders::SmoothRect(
                                  inset, inset, std::max(0.0, size.Width - 2 * inset),
                                  std::max(0.0, size.Height - 2 * inset), radius, kSmoothing));
                          });
                      }));
    }
    return holder;
}

// A smooth-cornered fill (Bloom's SmoothBorder) behind `content`, its size.
Controls::Grid Surface(FrameworkElement const& content, uint32_t fill, double radius,
                       Thickness padding) {
    Controls::Grid surface;
    Shapes::Path shape;
    shape.Fill(Brush(fill));
    content.Margin(padding);
    surface.Children().Append(Outline(surface, shape, radius, 0));
    surface.Children().Append(content);
    return surface;
}

// A line: `left` taking the room, `right` at its end.
Controls::Grid Line(FrameworkElement const& left, FrameworkElement const& right) {
    Controls::Grid line;
    Controls::ColumnDefinition rest, end;
    end.Width(GridLengthHelper::Auto());
    line.ColumnDefinitions().Append(rest);
    line.ColumnDefinitions().Append(end);
    line.Children().Append(left);
    if (right) {
        right.Margin({8, 0, 0, 0});
        Controls::Grid::SetColumn(right, 1);
        line.Children().Append(right);
    }
    return line;
}

Controls::StackPanel Across(std::initializer_list<FrameworkElement> items) {
    Controls::StackPanel row;
    row.Orientation(Controls::Orientation::Horizontal);
    for (auto const& item : items) {
        if (item) row.Children().Append(item);
    }
    return row;
}

std::wstring Joined(std::initializer_list<std::wstring> parts) {
    std::wstring out;
    for (auto const& part : parts) {
        if (part.empty()) continue;
        if (!out.empty()) out += L" · ";
        out += part;
    }
    return out;
}

// Bloom's home bar (HomeViews.Bar): 4 high, `fill` (the accent; red when the
// limit is nearly used up) on the track.
Controls::Grid Bar(double fraction, uint32_t fill, double width = 0) {
    Controls::Grid bar;
    bar.Height(4);
    if (width > 0) {
        bar.Width(width);
    }
    fraction = std::clamp(fraction, 0.0, 1.0);
    Controls::ColumnDefinition shown, rest;
    shown.Width({std::max(fraction, 0.0001), GridUnitType::Star});
    rest.Width({std::max(1 - fraction, 0.0001), GridUnitType::Star});
    bar.ColumnDefinitions().Append(shown);
    bar.ColumnDefinitions().Append(rest);
    auto radius = CornerRadiusHelper::FromUniformRadius(2);
    Controls::Border back;
    back.CornerRadius(radius);
    back.Background(Brush(C().track));
    Controls::Grid::SetColumnSpan(back, 2);
    bar.Children().Append(back);
    if (fraction > 0) {
        Controls::Border front;
        front.CornerRadius(radius);
        front.Background(Brush(fill));
        bar.Children().Append(front);
    }
    return bar;
}

// The local date and time of a Unix time: "11/12 (수) 18:00".
std::wstring LocalTime(int64_t unix) {
    ULARGE_INTEGER t;
    t.QuadPart = (uint64_t)(unix + 11644473600LL) * 10000000ULL;
    FILETIME file{t.LowPart, t.HighPart};
    SYSTEMTIME utc, local;
    if (unix <= 0 || !FileTimeToSystemTime(&file, &utc) ||
        !SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local)) {
        return {};
    }
    static constexpr PCWSTR kDays[] = {L"일", L"월", L"화", L"수", L"목", L"금", L"토"};
    WCHAR text[40];
    swprintf_s(text, L"%u/%u (%ls) %02u:%02u", local.wMonth, local.wDay,
               kDays[local.wDayOfWeek % 7], local.wHour, local.wMinute);
    return text;
}

// The account's logo as the meter draws it: its icon, else its letter on a
// tile, else its letter.
FrameworkElement Mark(usage::Account const& a, usage::Label const& label) {
    UINT px = (UINT)std::lround(kMarkSize * g_open->rasterization);
    auto icon = a.hasIcon ? usage::IconAt(a.icon, px) : nullptr;
    switch (usage::MarkOf(a, icon != nullptr)) {
        case usage::Mark::kIcon: {
            bool mono = settings::g.usage.logoColor != settings::LogoColor::kBrand;
            Controls::Image image;
            image.Width(kMarkSize);
            image.Height(kMarkSize);
            if (auto bitmap = usage::IconBitmap(
                    *usage::Colored(icon, mono, ToColor(mono ? C().dim : C().text)))) {
                image.Source(bitmap);
            }
            return image;
        }
        case usage::Mark::kTile: {
            Controls::Border tile;
            tile.Width(kMarkSize);
            tile.Height(kMarkSize);
            tile.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
            tile.Background(Brush(C().tile));
            auto letter = Text(label.letter, 10, C().text, Semi());
            letter.HorizontalAlignment(HorizontalAlignment::Center);
            tile.Child(letter);
            return tile;
        }
        default:
            return Text(label.initial, kNameSize, C().text, Semi());
    }
}

// A limit as Bloom's home shows it: its name, then "68%" (" 남음") and the time
// to its reset; under them its bar. The reset's date is in the tooltip. The
// numbers count the way Bloom shows them, whichever way the meter was turned.
FrameworkElement LimitRow(usage::Model const& m, usage::Account const& a, usage::Limit const& l,
                          int64_t now) {
    usage::Value v = usage::Evaluate(m, a, &l, now);
    v.left = m.left;
    bool number = v.kind == usage::Value::kNumber;
    bool hot = number && v.percent >= settings::theme::kUsageDangerPercent;
    Controls::StackPanel row;
    row.Margin({0, 2, 0, 0});
    Controls::TextBlock right;
    right.FontFamily(Media::FontFamily{kFont});
    right.VerticalAlignment(VerticalAlignment::Center);
    Documents::Typography::SetNumeralAlignment(right, FontNumeralAlignment::Tabular);
    if (hot) {  // a mark as well as the colour
        auto mark = Span(L"\xE7BA ", 10.5, C().danger);
        mark.FontFamily(Media::FontFamily{kIconFont});
        right.Inlines().Append(mark);
    }
    auto figure = Span(v.Figure() + (number ? L"%" : L""), kTextSize, hot ? C().danger : C().text, !hot);
    if (hot) {
        figure.FontWeight(Semi());
    }
    right.Inlines().Append(figure);
    if (number && v.left) {
        right.Inlines().Append(Span(L" 남음", kSmallSize, C().dim));
    }
    if (number && l.resetsAt > now) {
        right.Inlines().Append(Span(L"  " + usage::Duration(l.resetsAt - now), kSmallSize, C().faint));
    }
    row.Children().Append(
        Line(Text(l.name.empty() ? usage::WindowName(l.mins) : l.name, kSmallSize, C().dim), right));
    auto bar = Bar(number ? v.Shown() / 100.0 : 0, hot ? C().danger : C().accent);
    bar.Margin({0, 3, 0, 2});
    row.Children().Append(bar);
    if (std::wstring at = LocalTime(l.resetsAt); !at.empty()) {
        Controls::ToolTipService::SetToolTip(row, winrt::box_value(at + L" 초기화"));
    }
    return row;
}

// When the account's numbers came, or why they can't.
std::wstring Status(usage::Model const& m, usage::Account const& a, int64_t now) {
    if (!m.online) {
        return L"Bloom 꺼짐 · " + usage::Duration(std::max<int64_t>(0, now - m.writtenAt)) + L" 전";
    }
    if (usage::Age(a, now) > usage::kGiveUpSeconds) {
        return L"하루 넘게 갱신 없음";
    }
    return usage::Duration(usage::Age(a, now)) + L" 전";
}

// Bloom's home card: [mark][name][plan] … [when], then the limits; a signed-out
// account says so instead. Old numbers are dimmed.
FrameworkElement AccountCard(usage::Model const& m, usage::Account const& a,
                             usage::Label const& label, int64_t now) {
    Controls::StackPanel stack;
    Controls::Grid head;
    head.Margin({0, 0, 0, 5});
    for (int c = 0; c < 4; c++) {
        Controls::ColumnDefinition column;
        column.Width(c == 1 ? GridLength{1, GridUnitType::Star} : GridLengthHelper::Auto());
        head.ColumnDefinitions().Append(column);
    }
    auto mark = Mark(a, label);
    mark.Margin({0, 0, 6, 0});
    mark.VerticalAlignment(VerticalAlignment::Center);
    head.Children().Append(mark);
    auto name = Text(label.title, kNameSize, C().text, Semi());
    Controls::Grid::SetColumn(name, 1);
    head.Children().Append(name);
    if (!a.plan.empty()) {
        Controls::Border badge;
        badge.Height(16);
        badge.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
        badge.Background(Brush(C().tile));
        badge.Padding({5, 0, 5, 0});
        badge.Margin({6, 0, 0, 0});
        badge.VerticalAlignment(VerticalAlignment::Center);
        badge.Child(Text(a.plan, kSmallSize, C().dim, Medium()));
        Controls::Grid::SetColumn(badge, 2);
        head.Children().Append(badge);
    }
    if (!a.signedOut) {
        auto when = Text(Status(m, a, now), kSmallSize, C().faint);
        when.Margin({6, 0, 0, 0});
        Controls::ToolTipService::SetToolTip(when, winrt::box_value(L"마지막으로 받은 때"));
        Controls::Grid::SetColumn(when, 3);
        head.Children().Append(when);
    }
    stack.Children().Append(head);
    if (a.signedOut) {
        auto need = Text(L"로그인이 풀려 사용량을 받지 못했습니다", kSmallSize, C().dim);
        need.TextWrapping(TextWrapping::Wrap);
        stack.Children().Append(need);
    } else {
        Controls::StackPanel rows;
        rows.Opacity(usage::Dim(m, a, now) ? 0.55 : 1.0);
        for (auto const& l : a.limits) {
            rows.Children().Append(LimitRow(m, a, l, now));
        }
        if (a.limits.empty()) {
            rows.Children().Append(Text(L"받은 한도가 없습니다", kSmallSize, C().dim));
        }
        stack.Children().Append(rows);
    }
    return Surface(stack, C().card, kCardRadius, {kCardPadX, kCardPadY, kCardPadX, kCardPadY});
}

// ---- sessions --------------------------------------------------------------------------
// TokenCat's list, in Bloom's look: the sessions under their project folders,
// each in two lines with its subagents under it. A click on a session opens its
// details: its id, model and tool, and buttons to copy the command that resumes
// it and to show its folder.

constexpr size_t kMaxSessions = 12;
constexpr size_t kAgentsShown = 3;  // under a closed session; the rest in one line
// The list keeps one height, scrolling past it, so the popup never grows over
// the taskbar as sessions come and go.
constexpr double kSessionsHeight = 300;
constexpr int kLookEveryTicks = 2;  // a look every other second while open
constexpr int64_t kCopiedSeconds = 2;  // a copy button says so this long
constexpr double kIndent = 14;  // under the title, past the dot

bool SessionsOn() {
    return settings::g.usage.sessions;
}

// "4,020" below 100,000, then "123.4k", then "1.23M".
std::wstring Tokens(int n) {
    WCHAR text[32];
    if (n < 100000) {
        std::wstring digits = std::to_wstring(n), out;
        for (size_t i = 0; i < digits.size(); i++) {
            if (i && (digits.size() - i) % 3 == 0) {
                out += L',';
            }
            out += digits[i];
        }
        return out;
    }
    if (n < 999950) {
        swprintf_s(text, L"%.1fk", n / 1000.0);
    } else {
        swprintf_s(text, L"%.2fM", n / 1000000.0);
    }
    return text;
}

// "7.5k", "12k", "980".
std::wstring Compact(int n) {
    if (n < 1000) {
        return std::to_wstring(n);
    }
    WCHAR text[32];
    swprintf_s(text, L"%.1f", n < 999950 ? n / 1000.0 : n / 1000000.0);
    std::wstring out = text;
    if (out.ends_with(L".0")) {
        out.resize(out.size() - 2);
    }
    return out + (n < 999950 ? L"k" : L"M");
}

// "방금", "40초 전", "3분 전", "2시간 전", "1일 전".
std::wstring Ago(int64_t seconds) {
    seconds = std::max<int64_t>(0, seconds);
    if (seconds < 10) return L"방금";
    if (seconds < 60) return std::to_wstring(seconds / 10 * 10) + L"초 전";
    if (seconds < 3600) return std::to_wstring(seconds / 60) + L"분 전";
    if (seconds < 86400) return std::to_wstring(seconds / 3600) + L"시간 전";
    return std::to_wstring(seconds / 86400) + L"일 전";
}

// "0:41", "1:02:05".
std::wstring Elapsed(int64_t seconds) {
    seconds = std::max<int64_t>(0, seconds);
    WCHAR text[32];
    if (seconds >= 3600) {
        swprintf_s(text, L"%lld:%02lld:%02lld", seconds / 3600, seconds / 60 % 60, seconds % 60);
    } else {
        swprintf_s(text, L"%lld:%02lld", seconds / 60, seconds % 60);
    }
    return text;
}

// What a running tool is doing, by its name only.
std::wstring ToolWords(std::wstring const& name) {
    auto any = [&](std::initializer_list<PCWSTR> names) {
        return std::ranges::any_of(names, [&](PCWSTR n) { return name == n; });
    };
    if (name.starts_with(L"mcp__") || any({L"ListMcpResourcesTool", L"ReadMcpResourceTool"})) {
        return L"MCP 도구 실행";
    }
    if (any({L"Bash", L"BashOutput", L"exec", L"exec_command", L"shell", L"local_shell", L"js",
             L"unified_exec"})) {
        return L"명령 실행";
    }
    if (any({L"Read", L"Write", L"Edit", L"MultiEdit", L"NotebookEdit", L"Glob", L"Grep",
             L"apply_patch"})) {
        return L"파일 작업";
    }
    if (any({L"WebFetch", L"WebSearch", L"web_search", L"web_fetch"})) {
        return L"웹 조회";
    }
    if (any({L"Task", L"Agent", L"Workflow", L"SendMessage", L"ListAgents", L"spawn_agent",
             L"wait_agent", L"send_message", L"followup_task", L"list_agents", L"close_agent"})) {
        return L"하위 에이전트 대기";
    }
    return L"도구 실행";
}

std::wstring StateWords(ai_sessions::Row const& r) {
    using ai_sessions::State;
    switch (r.state) {
        case State::kInput:
            return r.tool == L"ExitPlanMode" ? L"계획 승인 대기" : L"질문 답변 대기";
        case State::kTool:
            return ToolWords(r.tool);
        case State::kWorking:
        case State::kOutput:
            return L"진행";
        case State::kStale:
            return L"로그 대기";
        case State::kComplete:
            return L"완료";
        case State::kInterrupted:
            return L"중단";
        case State::kUnfinished:
            return L"종료 기록 없음";
        default:
            return L"대기";
    }
}

std::wstring Client(ai_sessions::Row const& r) {
    return r.source == ai_sessions::Source::kCodex ? L"Codex" : L"Claude Code";
}

std::wstring ModelWords(ai_sessions::Row const& r) {
    return r.model + (r.effort.empty() || r.model.empty() ? L"" : L" " + r.effort);
}

// A session's key in the list's tags; empty for one without an id yet.
std::wstring KeyOf(ai_sessions::Row const& r) {
    if (r.sessionId.empty()) {
        return {};
    }
    return (r.source == ai_sessions::Source::kCodex ? L"x:" : L"c:") + r.sessionId;
}

// The state's dot: the accent while it runs, amber for a question, faint else.
uint32_t DotColor(ai_sessions::Row const& r) {
    using ai_sessions::State;
    switch (r.state) {
        case State::kInput:
            return C().warn;
        case State::kTool:
        case State::kWorking:
        case State::kOutput:
            return C().accent;
        case State::kStale:
            return C().dim;
        default:
            return C().faint;
    }
}

Controls::Border Dot(uint32_t color, double size) {
    Controls::Border dot;
    dot.Width(size);
    dot.Height(size);
    dot.CornerRadius(CornerRadiusHelper::FromUniformRadius(size / 2));
    dot.Background(Brush(color));
    dot.VerticalAlignment(VerticalAlignment::Center);
    return dot;
}

// `lead` at its size, `main` taking the rest, `end` at its size.
Controls::Grid Columns(FrameworkElement const& lead, FrameworkElement const& main,
                       FrameworkElement const& end) {
    Controls::Grid grid;
    Controls::ColumnDefinition first, rest, last;
    first.Width(GridLengthHelper::Auto());
    last.Width(GridLengthHelper::Auto());
    grid.ColumnDefinitions().Append(first);
    grid.ColumnDefinitions().Append(rest);
    grid.ColumnDefinitions().Append(last);
    if (lead) {
        grid.Children().Append(lead);
    }
    if (main) {
        Controls::Grid::SetColumn(main, 1);
        grid.Children().Append(main);
    }
    if (end) {
        end.Margin({8, 0, 0, 0});
        Controls::Grid::SetColumn(end, 2);
        grid.Children().Append(end);
    }
    return grid;
}

// Numbers that keep their width as they count.
Controls::TextBlock Figures() {
    Controls::TextBlock t;
    t.FontFamily(Media::FontFamily{kFont});
    t.VerticalAlignment(VerticalAlignment::Center);
    t.TextTrimming(TextTrimming::CharacterEllipsis);
    Documents::Typography::SetNumeralAlignment(t, FontNumeralAlignment::Tabular);
    return t;
}

void SetTag(FrameworkElement const& e, std::wstring const& tag) {
    e.Tag(winrt::box_value(winrt::hstring(tag)));
}

bool Copied(std::wstring const& tag, int64_t now) {
    return g_open->copied == tag && now - g_open->copiedAt <= kCopiedSeconds;
}

// A button on the details (Bloom's tile fill, lighter under the pointer).
FrameworkElement Action(std::wstring const& tag, PCWSTR glyph, std::wstring const& label,
                        int64_t now) {
    bool copied = Copied(tag, now);
    Controls::Border button;
    button.CornerRadius(CornerRadiusHelper::FromUniformRadius(6));
    button.Background(Brush(g_open->hover == tag ? C().edge : C().tile));
    button.Padding({9, 4, 10, 4});
    button.Margin({0, 0, 6, 0});
    auto icon = Text(copied ? L"\xE73E" : glyph, 11, copied ? C().accentText : C().dim);
    icon.FontFamily(Media::FontFamily{kIconFont});
    icon.Margin({0, 0, 6, 0});
    button.Child(Across({icon, Text(copied ? L"복사됨" : label, kSmallSize, C().text, Medium())}));
    SetTag(button, tag);
    Automation::AutomationProperties::SetName(button, label);
    return button;
}

// The PowerShell line that resumes a session in its folder, as TokenCat makes
// it; none for a subagent, a folder not known in full, or a character the
// shell could take as more than text.
std::wstring ResumeCommand(ai_sessions::Row const& r) {
    std::wstring const& path = r.projectPath;
    bool absolute = (path.size() > 2 && iswalpha(path[0]) && path[1] == L':' &&
                     (path[2] == L'\\' || path[2] == L'/')) ||
                    path.starts_with(L"\\\\");
    if (r.subagent || r.sessionId.empty() || !absolute) {
        return {};
    }
    for (WCHAR c : path + r.sessionId) {
        if (c < 0x20 || c == 0x7F || wcschr(L"&|<>^%", c)) {
            return {};
        }
    }
    if (!std::ranges::all_of(r.sessionId, [](WCHAR c) { return iswalnum(c) || c == L'-' || c == L'_'; })) {
        return {};
    }
    std::wstring quoted;
    for (WCHAR c : path) {
        if (wcschr(L"'‘’‚‛", c)) {
            quoted += c;  // doubled, it stays in the quotes
        }
        quoted += c;
    }
    return L"cd -LiteralPath '" + quoted + L"'; " +
           (r.source == ai_sessions::Source::kCodex ? L"codex resume " : L"claude --resume ") +
           r.sessionId;
}

bool CopyText(std::wstring const& text) {
    if (text.empty() || !OpenClipboard(folders::g_folderTaskbar)) {
        return false;
    }
    bool ok = false;
    if (EmptyClipboard()) {
        size_t bytes = (text.size() + 1) * sizeof(WCHAR);
        if (HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
            if (void* to = GlobalLock(memory)) {
                memcpy(to, text.c_str(), bytes);
                GlobalUnlock(memory);
                ok = SetClipboardData(CF_UNICODETEXT, memory) != nullptr;
            }
            if (!ok) {
                GlobalFree(memory);
            }
        }
    }
    CloseClipboard();
    return ok;
}

// "Explore a3e6d1ab · 파일 작업 … 1.2k tok · 2분 전"
FrameworkElement AgentLine(ai_sessions::Row const& a, int64_t now) {
    auto what = Figures();
    what.Inlines().Append(Span(a.role.empty() ? L"Subagent" : a.role, kSmallSize, C().dim, true));
    std::wstring id = a.agentId.size() > 8 && a.source == ai_sessions::Source::kClaude
                          ? a.agentId.substr(a.agentId.size() > 9 && a.agentId[0] == L'a' ? 1 : 0, 8)
                          : a.agentId;
    if (!id.empty()) {
        what.Inlines().Append(Span(L"  " + id, kSmallSize, C().faint));
    }
    what.Inlines().Append(Span(L" · " + StateWords(a), kSmallSize, C().faint));
    int tokens = a.Live() && a.turnOutput ? *a.turnOutput : a.lastTurnOutput;
    auto when = Text(Joined({tokens > 0 ? Compact(tokens) + L" tok" : L"", Ago(now - a.liveAt)}),
                     kSmallSize, C().faint);
    auto dot = Dot(DotColor(a), 5);
    dot.Margin({0, 0, 7, 0});
    auto line = Columns(dot, what, when);
    line.Margin({kIndent, 3, 0, 0});
    return line;
}

// The details under an open session: its id (copied by the button beside it),
// model, tool and when its log is written, then the two buttons.
FrameworkElement Details(ai_sessions::Row const& r, std::wstring const& key, int64_t now) {
    Controls::StackPanel panel;
    panel.Margin({kIndent, 8, 0, 2});
    panel.Spacing(3);
    panel.Background(Brush(0));  // a click here leaves it open
    SetTag(panel, L"none|");
    auto field = [&](PCWSTR label, std::wstring const& value, FrameworkElement const& end) {
        auto name = Text(label, kSmallSize, C().faint);
        name.Width(56);
        auto text = Text(value, kSmallSize, C().text);
        text.IsTextSelectionEnabled(false);
        panel.Children().Append(Columns(name, text, end));
    };
    std::wstring copyTag = L"copy|" + key;
    bool copied = Copied(copyTag, now);
    Controls::Border copy;
    copy.Width(22);
    copy.Height(20);
    copy.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    copy.Background(Brush(g_open->hover == copyTag ? C().edge : 0));
    auto glyph = Text(copied ? L"\xE73E" : L"\xE8C8", 11, copied ? C().accentText : C().dim);
    glyph.FontFamily(Media::FontFamily{kIconFont});
    glyph.HorizontalAlignment(HorizontalAlignment::Center);
    copy.Child(glyph);
    SetTag(copy, copyTag);
    Automation::AutomationProperties::SetName(copy, L"세션 ID 복사");
    Controls::ToolTipService::SetToolTip(copy, winrt::box_value(L"세션 ID 복사"));
    field(L"세션 ID", r.sessionId, copy);
    field(L"모델", r.model.empty() ? L"기록 대기" : ModelWords(r), nullptr);
    if (!r.tool.empty()) {
        field(L"도구", r.tool, nullptr);
    }
    field(L"기록 시점",
          Client(r) + (r.source == ai_sessions::Source::kCodex ? L"는 응답 완료 시 기록"
                                                                : L"는 메시지 완료 시 기록"),
          nullptr);
    Controls::StackPanel actions;
    actions.Orientation(Controls::Orientation::Horizontal);
    actions.Margin({0, 6, 0, 0});
    if (!ResumeCommand(r).empty()) {
        actions.Children().Append(Action(L"resume|" + key, L"\xE8C8", L"재개 명령 복사", now));
    }
    if (!r.projectPath.empty()) {
        actions.Children().Append(Action(L"open|" + key, L"\xE838", L"탐색기에서 보기", now));
    }
    if (actions.Children().Size()) {
        panel.Children().Append(actions);
    }
    return panel;
}

// ● title … 4,020 tok · 0:41
//   명령 실행 · Bash · model · +1,120 tok · 1분 전 … ▬ 61%
// then its subagents, and its details when open.
FrameworkElement SessionItem(ai_sessions::Row const& r, int64_t now) {
    std::wstring key = KeyOf(r);
    bool open = !key.empty() && g_open->expanded == key;
    Controls::StackPanel stack;

    auto dot = Dot(DotColor(r), 6);
    dot.VerticalAlignment(VerticalAlignment::Top);
    dot.Margin({1, 6, 7, 0});
    std::wstring name = !r.title.empty() ? r.title
                        : !r.subagent    ? L"제목 없는 세션"
                                         : (r.role.empty() ? L"" : r.role + L" ") + L"하위 에이전트";
    auto title = Text(name, kNameSize, C().text, Semi());
    title.TextWrapping(TextWrapping::Wrap);
    title.MaxLines(2);
    title.VerticalAlignment(VerticalAlignment::Top);
    auto figures = Figures();
    figures.VerticalAlignment(VerticalAlignment::Top);
    figures.Margin({0, 1, 0, 0});
    int tokens = r.Live() ? r.turnOutput.value_or(0) : r.lastTurnOutput;
    if (tokens > 0) {
        figures.Inlines().Append(Span(Tokens(tokens), kTextSize, C().text, true));
        figures.Inlines().Append(Span(L" tok", kSmallSize, C().dim));
    }
    std::wstring timer;
    if (r.state == ai_sessions::State::kInput) {
        timer = L"대기 " + Elapsed(now - r.liveAt);
    } else if (r.Live() && r.state != ai_sessions::State::kStale && r.turnStartedAt) {
        timer = Elapsed(now - r.turnStartedAt);
    }
    if (!timer.empty()) {
        figures.Inlines().Append(Span((tokens > 0 ? L" · " : L"") + timer, kSmallSize, C().faint));
    }
    stack.Children().Append(Columns(dot, title, figures));

    bool output = r.lastOutputDelta > 0 && r.lastOutputAt;
    std::wstring state = Joined({StateWords(r), r.state == ai_sessions::State::kTool ? r.tool : L"",
                                 ModelWords(r),
                                 output ? L"+" + Tokens(r.lastOutputDelta) + L" tok" : L"",
                                 Ago(now - r.liveAt)});
    FrameworkElement context{nullptr};
    if (r.contextUsed >= 0 && r.contextWindow > 0) {
        int percent = (int)std::lround(100.0 * r.contextUsed / r.contextWindow);
        bool full = percent >= 85;
        auto bar = Bar((double)r.contextUsed / r.contextWindow, full ? C().warn : C().accent, 32);
        bar.Margin({0, 0, 5, 0});
        bar.VerticalAlignment(VerticalAlignment::Center);
        context = Across({bar, Text(std::to_wstring(percent) + L"%", kSmallSize,
                                    full ? C().warn : C().faint)});
        Controls::ToolTipService::SetToolTip(context, winrt::box_value(L"컨텍스트 " + Compact(r.contextUsed) +
                                                                            L" / " + Compact(r.contextWindow)));
    }
    auto second = Columns(nullptr, Text(state, kSmallSize, C().dim), context);
    second.Margin({kIndent, 2, 0, 0});
    stack.Children().Append(second);

    size_t shown = open ? r.agents.size() : std::min(r.agents.size(), kAgentsShown);
    for (size_t i = 0; i < shown; i++) {
        stack.Children().Append(AgentLine(r.agents[i], now));
    }
    if (shown < r.agents.size()) {
        int64_t last = 0;
        for (size_t i = shown; i < r.agents.size(); i++) {
            last = std::max(last, r.agents[i].liveAt);
        }
        auto more = Text(L"+" + std::to_wstring(r.agents.size() - shown) + L"개 더 · 마지막 기록 " +
                             Ago(now - last),
                         kSmallSize, C().faint);
        more.Margin({kIndent + 12, 3, 0, 0});
        stack.Children().Append(more);
    }
    if (open) {
        stack.Children().Append(Details(r, key, now));
    }

    Controls::Border item;
    item.CornerRadius(CornerRadiusHelper::FromUniformRadius(8));
    item.Padding({8, 7, 8, 8});
    std::wstring tag = key.empty() ? L"" : L"row|" + key;
    item.Background(Brush(open                                    ? C().accentSoft
                          : !tag.empty() && g_open->hover == tag ? C().hover
                                                                 : 0));
    item.Child(stack);
    if (!tag.empty()) {
        SetTag(item, tag);
        Automation::AutomationProperties::SetName(item, name);
    }
    return item;
}

// "입력 필요 2 · 진행 1 · 에이전트 3", else nothing running.
std::wstring Summary(std::vector<ai_sessions::Row> const& rows) {
    int input = 0, running = 0, agents = 0;
    for (auto const& r : rows) {
        if (r.state == ai_sessions::State::kInput) {
            input++;
        } else if (r.Live() && r.state != ai_sessions::State::kStale) {
            running++;
        }
        agents += (int)std::ranges::count_if(r.agents, [](ai_sessions::Row const& a) {
            return a.Live() && a.state != ai_sessions::State::kStale;
        });
    }
    return Joined({input ? L"입력 필요 " + std::to_wstring(input) : L"",
                   running ? L"진행 " + std::to_wstring(running) : L"",
                   agents ? L"에이전트 " + std::to_wstring(agents) : L""});
}

// The tag of what the pointer is on: the nearest tagged element up from it.
std::wstring TagAt(wf::IInspectable const& source) {
    auto at = source.try_as<DependencyObject>();
    for (int up = 0; at && up < 16; up++) {
        if (auto element = at.try_as<FrameworkElement>()) {
            auto tag = winrt::unbox_value_or<winrt::hstring>(element.Tag(), L"");
            if (!tag.empty()) {
                return std::wstring(tag);
            }
        }
        at = Media::VisualTreeHelper::GetParent(at);
    }
    return {};
}

void RenderSessions();

ai_sessions::Row const* RowByKey(ai_sessions::Snapshot const* snapshot, std::wstring const& key) {
    if (!snapshot || key.empty()) {
        return nullptr;
    }
    for (auto const& r : snapshot->rows) {
        if (KeyOf(r) == key) {
            return &r;
        }
    }
    return nullptr;
}

// A click on the list, by the tag it landed on.
void Act(std::wstring const& tag) {
    size_t bar = tag.find(L'|');
    if (bar == std::wstring::npos || !Ours()) {
        return;
    }
    std::wstring what = tag.substr(0, bar), key = tag.substr(bar + 1);
    auto snapshot = ai_sessions::g_snapshot;  // held while its row is used
    if (what == L"none") {
        return;
    } else if (what == L"row") {
        g_open->expanded = g_open->expanded == key ? L"" : key;
    } else if (auto const* r = RowByKey(snapshot.get(), key)) {
        if (what == L"copy" || what == L"resume") {
            if (CopyText(what == L"copy" ? r->sessionId : ResumeCommand(*r))) {
                g_open->copied = tag;
                g_open->copiedAt = usage::Now();
            }
        } else if (what == L"open" && !r->projectPath.empty()) {
            folders::StartJob([path = r->projectPath] {
                DWORD attributes = GetFileAttributesW(path.c_str());
                if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    folders::OpenOrSwitch(nullptr, path);
                }
            });
        }
    }
    RenderSessions();
}

// One set of pointer handlers on the list, kept for the opening; the rows
// under them are made again every second, so they carry tags, not handlers. A
// click counts when it lets go on what it pressed (the same tag, as the row
// may have been made again in between).
void WatchList(Controls::StackPanel const& list) {
    if (!folders::g_popup) {
        return;
    }
    auto& p = *folders::g_popup;
    folders::Keep(p, list.PointerPressed(winrt::auto_revoke, [](auto&&, Input::PointerRoutedEventArgs const& e) {
        core::Guard(L"Usage list press", [&] {
            if (Ours() && e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed()) {
                g_open->pressed = TagAt(e.OriginalSource());
            }
        });
    }));
    folders::Keep(p, list.PointerReleased(winrt::auto_revoke, [](auto&&, Input::PointerRoutedEventArgs const& e) {
        core::Guard(L"Usage list release", [&] {
            if (!Ours()) {
                return;
            }
            std::wstring tag = TagAt(e.OriginalSource());
            bool same = !tag.empty() && tag == g_open->pressed;
            g_open->pressed.clear();
            if (same) {
                core::Post([tag] { core::Guard(L"Usage list click", [&] { Act(tag); }); });
            }
        });
    }));
    auto hover = [](std::wstring tag) {
        if (!Ours() || tag == g_open->hover) {
            return;
        }
        g_open->hover = std::move(tag);
        core::Post([] {
            core::Guard(L"Usage list hover", [] {
                if (Ours()) {
                    RenderSessions();
                }
            });
        });
    };
    folders::Keep(p, list.PointerMoved(winrt::auto_revoke, [hover](auto&&, Input::PointerRoutedEventArgs const& e) {
        core::Guard(L"Usage list move", [&] { hover(TagAt(e.OriginalSource())); });
    }));
    folders::Keep(p, list.PointerExited(winrt::auto_revoke, [hover](auto&&, auto&&) {
        core::Guard(L"Usage list leave", [&] { hover(L""); });
    }));
}

// "세션 … 입력 필요 1 · 진행 2", then the list on a card of a fixed height.
FrameworkElement BuildSessions() {
    Controls::StackPanel panel;
    auto summary = Text(L"", kSmallSize, C().dim);
    auto head = Line(Text(L"세션", kNameSize, C().text, Semi()), summary);
    head.Margin({2, 14, 2, 0});
    panel.Children().Append(head);
    Controls::StackPanel list;
    list.Margin({6, 6, 6, 6});
    list.Spacing(2);
    list.Background(Brush(0));  // its gaps take the pointer too
    Controls::ScrollViewer scroll;
    scroll.Height(kSessionsHeight);
    scroll.VerticalScrollBarVisibility(Controls::ScrollBarVisibility::Auto);
    scroll.HorizontalScrollBarVisibility(Controls::ScrollBarVisibility::Disabled);
    scroll.Content(list);
    auto card = Surface(scroll, C().card, kCardRadius, {0, 0, 0, 0});
    card.Margin({0, kGap, 0, 0});
    panel.Children().Append(card);
    g_open->sessionsSummary = summary;
    g_open->sessionsList = list;
    WatchList(list);
    return panel;
}

void LetGo();

// Lays the open popup out now, from here. A layout that doesn't settle (XAML's
// layout cycle) then comes back as an error to catch, where XAML's own pass
// would end explorer with it; the popup goes instead. False when it went.
bool Settle(PCWSTR when) {
    if (!Ours() || !folders::g_popup->content) {
        return true;
    }
    try {
        folders::g_popup->content.UpdateLayout();
        return true;
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Usage details layout failed (%s): %08X", when, hr);
        core::DiagLine(L"Usage details layout failed (%s): %08X", when, hr);
        LetGo();
        return false;
    }
}

// The rows again, with the current time, under their project folders in the
// order of their first session: only the list's content changes, so it keeps
// where it was scrolled to.
void DrawSessions() {
    if (!Ours()) {
        return;
    }
    auto list = g_open->sessionsList;
    auto summary = g_open->sessionsSummary;
    if (!list || !summary) {
        return;
    }
    auto snapshot = ai_sessions::g_snapshot;
    int64_t now = usage::Now();
    size_t count = snapshot ? std::min(snapshot->rows.size(), kMaxSessions) : 0;
    std::vector<ai_sessions::Row> rows;
    if (snapshot) {
        rows.assign(snapshot->rows.begin(), snapshot->rows.begin() + count);
    }
    summary.Text(Summary(rows));
    if (g_open->renders++ < 2) {
        core::DiagLine(L"Usage details rows %u", (unsigned)rows.size());
    }
    list.Children().Clear();
    if (rows.empty()) {
        auto none = Text(snapshot ? L"최근 하루 동안의 Claude Code·Codex 세션이 없습니다"
                                  : L"세션 기록을 읽는 중…",
                         kSmallSize, C().dim);
        none.TextWrapping(TextWrapping::Wrap);
        none.Margin({8, 6, 8, 6});
        list.Children().Append(none);
        return;
    }
    std::vector<std::pair<std::wstring, std::vector<size_t>>> groups;
    for (size_t i = 0; i < rows.size(); i++) {
        std::wstring name = rows[i].project.empty() ? L"프로젝트 미확인" : rows[i].project;
        auto group = std::ranges::find_if(groups, [&](auto const& g) { return g.first == name; });
        if (group == groups.end()) {
            groups.push_back({name, {i}});
        } else {
            group->second.push_back(i);
        }
    }
    for (size_t g = 0; g < groups.size(); g++) {
        auto icon = Text(L"\xE8B7", 11, C().faint);
        icon.FontFamily(Media::FontFamily{kIconFont});
        icon.Margin({0, 0, 6, 0});
        auto name = Text(groups[g].first, kSmallSize, C().dim, Semi());
        auto number = Text(std::to_wstring(groups[g].second.size()), kSmallSize, C().faint);
        auto head = Columns(icon, name, number);
        head.Margin({8, g ? 10.0 : 2.0, 8, 2});
        list.Children().Append(head);
        for (size_t i : groups[g].second) {
            list.Children().Append(SessionItem(rows[i], now));
        }
    }
}

void RenderSessions() {
    DrawSessions();
    Settle(L"sessions");
}

void StopSessions() {
    ai_sessions::g_onRows = nullptr;
    if (g_open && g_open->tick) {
        g_open->tick.Stop();
        g_open->tick = nullptr;
    }
    if (ai_sessions::g_wanted.exchange(false)) {
        ai_sessions::Forget();
    }
}

// While the details are open: the times every second, a look every other.
void StartSessions() {
    if (!SessionsOn()) {
        return;
    }
    ai_sessions::g_wanted = true;
    ai_sessions::g_findNow = true;
    ai_sessions::g_onRows = RenderSessions;
    ai_sessions::Request();
    DispatcherTimer tick;
    tick.Interval(std::chrono::seconds(1));
    tick.Tick([](auto&&, auto&&) {
        core::Guard(L"Usage sessions", [] {
            if (!Ours() || !folders::g_popup->flyout || !folders::g_popup->flyout.IsOpen()) {
                return StopSessions();
            }
            RenderSessions();
            if (!Ours()) {
                return;  // let go of: its layout failed
            }
            if (++g_open->ticks % kLookEveryTicks == 0) {
                ai_sessions::Request();
            }
        });
    });
    g_open->tick = tick;
    tick.Start();
}

// ---- the popup -------------------------------------------------------------------------

// The popup's content, made again whenever the numbers or their way change.
void Fill(Controls::StackPanel const& body) {
    body.Children().Clear();
    g_open->sessionsSummary = nullptr;
    g_open->sessionsList = nullptr;
    auto model = usage::g_model;
    int64_t now = usage::Now();

    auto head = Text(L"AI 사용량", kTitleSize, C().text, Semi());
    head.Margin({2, 0, 0, 2});
    body.Children().Append(head);

    if (!model || model->accounts.empty()) {
        auto none = Text(L"Bloom이 내보낸 사용량이 없습니다", kSmallSize, C().dim);
        none.Margin({2, 6, 0, 2});
        body.Children().Append(none);
    }
    auto labels = model ? usage::Labels(*model) : std::vector<usage::Label>{};
    for (size_t i = 0; model && i < model->accounts.size(); i++) {
        auto card = AccountCard(*model, model->accounts[i], labels[i], now);
        card.Margin({0, kGap, 0, 0});
        body.Children().Append(card);
    }
    if (SessionsOn()) {
        body.Children().Append(BuildSessions());
        DrawSessions();
    }
}

// Bloom's popup surface (PopupMenu): its popup fill with a hairline edge in the
// smooth-cornered shape, lifted by a soft shadow, holding `content`. In the
// room the folder popup keeps around its bubble, so it sits where theirs do.
FrameworkElement Bubble(folders::Popup& p, FrameworkElement const& content) {
    Controls::Grid shell;
    Controls::Border shadowHost;
    Shapes::Path body;
    body.Fill(Brush(C().popup));
    body.Stroke(Brush(C().popupLine));
    body.StrokeThickness(1);
    p.body = body;
    content.Margin({kPadding, kPadding, kPadding, kPadding});
    shell.Children().Append(shadowHost);
    shell.Children().Append(Outline(shell, body, kPopupRadius, 0.5));
    shell.Children().Append(content);
    auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(shadowHost).Compositor();
    auto sprite = compositor.CreateSpriteVisual();
    auto shadow = compositor.CreateDropShadow();
    shadow.Mask(body.GetAlphaMask());
    shadow.BlurRadius(kShadowBlur);
    shadow.Offset({0, (float)kShadowDepth, 0});
    shadow.Opacity((float)kShadowOpacity);
    sprite.Shadow(shadow);
    Hosting::ElementCompositionPreview::SetElementChildVisual(shadowHost, sprite);
    folders::Keep(p, shell.SizeChanged(winrt::auto_revoke, [sprite](
                                                               auto&&, SizeChangedEventArgs const& e) {
        core::Guard(L"Usage shadow", [&] { sprite.Size({e.NewSize().Width, e.NewSize().Height}); });
    }));
    Controls::Grid room;
    room.Padding({folders::kShadowRoom, folders::kShadowRoom, folders::kShadowRoom,
                  folders::kShadowRoom});
    room.Children().Append(shell);
    p.content = shell;
    return room;
}

// Everything one opening made goes: the popup's XAML, the session rows, what
// was read for them.
void LetGo() {
    StopSessions();
    if (Ours()) {
        folders::ClosePopup();
    }
    g_open.reset();
}

// While the popup shows, an error XAML meets in its own pass (which would end
// explorer) is written to the diagnostics in XAML's words. Where explorer's
// XAML has an application, the error is marked handled and the popup goes.
void WatchErrors(folders::Popup& p) {
    try {
        if (auto app = Application::Current()) {
            folders::Keep(p, app.UnhandledException(
                                 winrt::auto_revoke, [](auto&&, UnhandledExceptionEventArgs const& e) {
                                     core::Guard(L"Usage details error", [&] {
                                         if (!Ours()) {
                                             return;
                                         }
                                         core::DiagLine(L"Usage details XAML error %08X: %s",
                                                        (int32_t)e.Exception(), e.Message().c_str());
                                         e.Handled(true);
                                         core::Post([] {
                                             if (Ours()) {
                                                 LetGo();
                                             }
                                         });
                                     });
                                 }));
        }
    } catch (...) {
    }
    try {
        folders::Keep(p, winrt::Windows::ApplicationModel::Core::CoreApplication::UnhandledErrorDetected(
                             winrt::auto_revoke, [](auto&&, auto const& e) {
                                 if (!Ours()) {
                                     return;
                                 }
                                 try {
                                     e.UnhandledError().Propagate();  // only to read it
                                 } catch (...) {
                                     core::DiagLine(L"Usage details XAML error %08X: %s",
                                                    winrt::to_hresult(), winrt::to_message().c_str());
                                 }
                             }));
    } catch (...) {
    }
}

void Refresh() {
    if (!Ours()) {
        return;
    }
    if (auto body = g_open->body) {
        Fill(body);
        Settle(L"refresh");
    }
}

// Opens the details over the meter, or closes them when the click that just
// closed them was on the meter.
void Toggle(HWND taskbar, FrameworkElement const& anchor) {
    if (folders::JustClosed(kKey)) {
        return;
    }
    if (Ours() && folders::g_popup->flyout && folders::g_popup->flyout.IsOpen()) {
        return folders::HideSelf(*folders::g_popup);
    }
    LetGo();
    folders::NoteTaskbarThread();
    if (!core::g_popupDropsTimer) {
        core::g_popupDropsTimer = folders::PopupDropsTimer;
        g_ownsTimerHook = true;
    }
    POINT pt;
    GetCursorPos(&pt);
    folders::g_folderTaskbar = taskbar;
    folders::Popup& p = folders::NewPopup(kKey, anchor);
    p.look.scale = 100;  // Bloom's sizes, not the folder popup's
    Open& open = g_open.emplace();
    open.dark = p.colors != &folders::kLightPalette;
    open.colors = MakeColors(open.dark);
    auto root = anchor.XamlRoot();
    open.rasterization = root ? root.RasterizationScale() : 1.0;
    Controls::StackPanel body;
    body.Width(kWidth);
    open.body = body;
    core::DiagLine(L"Usage details open");
    try {
        Fill(body);
        Controls::Flyout flyout;
        flyout.FlyoutPresenterStyle(folders::BarePresenter());
        flyout.Content(folders::Scaled(p, Bubble(p, body)));
        p.flyout = flyout;
        folders::Present(p, anchor, folders::TaskbarOnTop(pt));
        folders::Keep(p, flyout.Opened(winrt::auto_revoke, [](auto&&, auto&&) {
            core::Guard(L"Usage details opened", [] {
                if (Ours() && folders::g_popup->content && Settle(L"opened")) {
                    core::DiagLine(L"Usage details shown %.0fx%.0f",
                                   folders::g_popup->content.ActualWidth(),
                                   folders::g_popup->content.ActualHeight());
                }
            });
        }));
        // Once closed it is let go of, after the folder popup's own handlers.
        folders::Keep(p, flyout.Closed(winrt::auto_revoke, [](auto&&, auto&&) {
            core::Guard(L"Usage details closed", [] {
                StopSessions();
                core::Post([] {
                    if (Ours() && !(folders::g_popup->flyout && folders::g_popup->flyout.IsOpen())) {
                        LetGo();
                    }
                });
            });
        }));
        WatchErrors(p);
        if (!Settle(L"open")) {
            return;
        }
        StartSessions();
    } catch (...) {
        Wh_Log(L"Usage details failed: %08X", winrt::to_hresult());
        core::DiagLine(L"Usage details failed: %08X", winrt::to_hresult());
        LetGo();
    }
}

// The meter goes (an unload, new settings): its popup and everything it set up
// with it, which the folders otherwise undo on their own unload.
void Close() {
    LetGo();
    folders::DropGiveBack();
    if (g_ownsTimerHook) {
        core::g_popupDropsTimer = nullptr;
        folders::g_popupHeld = nullptr;
        g_ownsTimerHook = false;
    }
}

}  // namespace usage_detail

// ============================================================================
namespace panels {
// ============================================================================
// Quick Settings (Win+A) and the notification center with the calendar (Win+N, the
// clock) open at the right end of the screen, where the stock tray is. Here they
// open at the pill's tray: the panel's right edge on the tray's, inside the work
// area. Explorer keeps each taskbar's tray span on the taskbar window. Each panel is
// placed before it shows, by the process that places it: Quick Settings places its
// own window in ShellHost.exe (SetWindowPos), and explorer places the notification
// center's window and then uncloaks it (DwmSetWindowAttribute). Quick Settings opens
// at a top taskbar by itself; the notification center stays at the bottom, so under
// a top taskbar explorer moves it up, by the region it cuts the window to
// (SetWindowRgn).

// Screen x of the tray's left and right edges, packed into a window property (left
// in the low half). ShellHost reads it too.
constexpr WCHAR kTrayProp[] = L"PillTaskbar.TraySpan";

ULONG_PTR Pack(LONG left, LONG right) {
    return (ULONG_PTR)(UINT32)left | ((ULONG_PTR)(UINT32)right << 32);
}

// The taskbar window on a monitor. Works from any process.
HWND TaskbarOn(HMONITOR monitor) {
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        for (HWND hwnd = nullptr; (hwnd = FindWindowEx(nullptr, hwnd, cls, nullptr));) {
            // As core::MonitorOf: a hidden top taskbar lies mostly on the monitor above.
            auto on = (HMONITOR)GetProp(hwnd, L"TaskbarMonitor");
            if ((on ? on : MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)) == monitor) {
                return hwnd;
            }
        }
    }
    return nullptr;
}

// Where a panel of this size goes on the monitor it opens on: x for its right edge
// at the tray's, kept inside the work area. x itself where no tray span is kept
// (the feature is off in explorer, or the taskbar is vertical).
int PlaceX(int x, int y, int width, int height) {
    RECT rc{x, y, x + width, y + height};
    HMONITOR monitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
    HWND taskbar = TaskbarOn(monitor);
    auto span = taskbar ? (ULONG_PTR)GetProp(taskbar, kTrayProp) : 0;
    LONG left = (LONG)(UINT32)span, right = (LONG)(UINT32)(span >> 32);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (right <= left || !GetMonitorInfo(monitor, &mi) || left < mi.rcMonitor.left ||
        right > mi.rcMonitor.right) {
        return x;
    }
    LONG lo = mi.rcWork.left, hi = mi.rcWork.right - width;
    return hi < lo ? x : std::clamp<LONG>(right - width, lo, hi);
}

// ---- explorer: the tray span (taskbar thread) ------------------------------------

struct Bar {
    HWND hwnd = nullptr;
    winrt::weak_ref<FrameworkElement> frame;
    FrameworkElement::LayoutUpdated_revoker layoutUpdated;
    ULONG_PTR span = 0;  // set on the window, 0 if none
};

std::list<Bar> g_bars;  // taskbar thread only

// On every layout pass, as the pill grows or moves; the window property is written
// only when the span changed.
void Publish(Bar& bar) {
    auto frame = bar.frame.get();
    auto root = frame ? frame.XamlRoot() : nullptr;
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    RECT window;
    if (!content || !GetWindowRect(bar.hwnd, &window)) {
        return;
    }
    // A taskbar without a tray (a secondary monitor's) has its whole pill instead.
    FrameworkElement piece =
        core::ChildOfClass(core::Parent(frame), L"SystemTray.SystemTrayFrame");
    if (!piece || piece.Visibility() != Visibility::Visible || piece.ActualWidth() <= 0) {
        piece = frame;
    }
    if (piece.ActualWidth() <= 0) {
        return;  // not laid out yet
    }
    // DIPs to window pixels by the window DPI, as region::Update.
    UINT dpi = GetDpiForWindow(bar.hwnd);
    double scale = dpi ? dpi / 96.0 : root.RasterizationScale();
    auto r = piece.TransformToVisual(content).TransformBounds(
        {0, 0, (float)piece.ActualWidth(), (float)piece.ActualHeight()});
    LONG left = window.left + (LONG)std::lround(r.X * scale);
    LONG right = window.left + (LONG)std::lround((r.X + r.Width) * scale);
    ULONG_PTR span = right > left ? Pack(left, right) : 0;
    if (span && span != bar.span && SetProp(bar.hwnd, kTrayProp, (HANDLE)span)) {
        bar.span = span;
    }
}

void Apply(HWND hwnd) {
    if (core::g_vertical) {
        return;
    }
    auto root = core::TaskbarRoot(hwnd);
    auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
    auto frame = content ? core::FindDescendant(
                               content,
                               [](auto& e) { return core::IsClass(e, L"Taskbar.TaskbarFrame"); }, 8)
                         : nullptr;
    if (!frame) {
        return;
    }
    std::erase_if(g_bars, [&](Bar& b) {
        auto f = b.frame.get();
        return !f || (b.hwnd == hwnd && f != frame);
    });
    for (auto& bar : g_bars) {
        if (bar.hwnd == hwnd) {
            Publish(bar);
            return;
        }
    }
    auto& bar = g_bars.emplace_back();
    bar.hwnd = hwnd;
    bar.frame = frame;
    Bar* p = &bar;  // std::list keeps it in place
    bar.layoutUpdated = frame.LayoutUpdated(
        winrt::auto_revoke,
        [p](auto&&, auto&&) { core::Guard(L"Tray span", [&] { Publish(*p); }); });
    Publish(bar);
}

// ---- explorer: the notification center ---------------------------------------------

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
using SetWindowRgn_t = decltype(&SetWindowRgn);
SetWindowRgn_t SetWindowRgn_Original;
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags);

// The notification center's window, where Windows had it and where it was put, so it
// can go back when the mod stops. Explorer may leave it where it was between openings.
std::mutex g_centerLock;
HWND g_center;
POINT g_centerDefault, g_centerAt;

// The gap Windows leaves between a panel and a bottom taskbar, at 100%.
constexpr int kPanelGap = 12;

// Screen y for the top of a panel's shown part (on screen) under a top taskbar: the
// gap below the taskbar, a hidden one's height counted as Quick Settings counts it,
// kept inside the work area. Its own top where the monitor's taskbar is not at the
// top. A hidden top taskbar lies mostly above the monitor and counts all the same.
LONG PlaceTop(const RECT& shown) {
    HMONITOR monitor = MonitorFromRect(&shown, MONITOR_DEFAULTTONEAREST);
    HWND taskbar = TaskbarOn(monitor);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    RECT bar;
    if (!taskbar || !GetWindowRect(taskbar, &bar) || !GetMonitorInfo(monitor, &mi) ||
        bar.right - bar.left <= bar.bottom - bar.top ||
        bar.top + bar.bottom >= mi.rcMonitor.top + mi.rcMonitor.bottom) {
        return shown.top;
    }
    UINT dpi = GetDpiForWindow(taskbar);  // explorer's, on that monitor
    LONG top = std::max<LONG>(mi.rcWork.top, mi.rcMonitor.top + (bar.bottom - bar.top)) +
               MulDiv(kPanelGap, dpi ? dpi : 96, 96);
    return std::min<LONG>(top, mi.rcWork.bottom - (shown.bottom - shown.top));
}

bool HasName(PCWSTR path, PCWSTR name) {
    PCWSTR slash = wcsrchr(path, L'\\');
    return _wcsicmp(slash ? slash + 1 : path, name) == 0;
}

// The CoreWindow on the "ActionCenter" thread of ShellExperienceHost (or ShellHost).
bool IsNotificationCenter(HWND hwnd) {
    WCHAR cls[32];
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (!tid || !GetClassName(hwnd, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    bool center = false;
    if (HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid)) {
        PWSTR description = nullptr;
        if (SUCCEEDED(GetThreadDescription(thread, &description)) && description) {
            center = wcscmp(description, L"ActionCenter") == 0;
            LocalFree(description);
        }
        CloseHandle(thread);
    }
    if (!center) {
        return false;
    }
    center = false;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        WCHAR path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        center = QueryFullProcessImageName(process, 0, path, &size) &&
                 (HasName(path, L"ShellExperienceHost.exe") || HasName(path, L"ShellHost.exe"));
        CloseHandle(process);
    }
    return center;
}

// The window is as tall as the work area; Windows shows only the panel through the
// window's region, at the bottom. The panel goes to the tray and, under a top taskbar,
// to the top. Where the window goes, with it at rc where explorer has or puts it;
// false to leave it. region: the one being set, or null for the window's own.
bool CenterTarget(HWND hwnd, HRGN region, const RECT& rc, POINT& at) {
    RECT panel;  // in the window
    int kind = region ? GetRgnBox(region, &panel) : GetWindowRgnBox(hwnd, &panel);
    if (kind == NULLREGION) {
        return false;
    }
    bool cut = kind != ERROR;
    if (!cut) {
        panel = {0, 0, rc.right - rc.left, rc.bottom - rc.top};  // no region: y stays
    }
    if (panel.right - panel.left <= 1 || panel.bottom - panel.top <= 1) {
        return false;  // a placeholder while ShellExperienceHost builds its views
    }
    LONG x, y;
    {
        std::lock_guard lock(g_centerLock);
        // Where explorer put it, kept per coordinate it set this time.
        if (hwnd != g_center) {
            g_center = hwnd;
            g_centerDefault = {rc.left, rc.top};
        } else {
            if (rc.left != g_centerAt.x) g_centerDefault.x = rc.left;
            if (rc.top != g_centerAt.y) g_centerDefault.y = rc.top;
        }
        // The panel where explorer put the window picks the monitor. Where the window is
        // now won't do: moved up for a short panel, the panel's next, taller region can
        // reach mostly into the monitor above.
        RECT shown = panel;
        OffsetRect(&shown, g_centerDefault.x, g_centerDefault.y);
        x = PlaceX(shown.left, shown.top, shown.right - shown.left, shown.bottom - shown.top) -
            panel.left;
        y = cut ? PlaceTop(shown) - panel.top : rc.top;
        g_centerAt = {x, y};
    }
    at = {x, y};
    return true;
}

thread_local bool g_placing;  // the mod's own move, which the SetWindowPos hook lets by

void PlaceNotificationCenter(HWND hwnd, HRGN region) {
    RECT rc;
    POINT at;
    if (!IsNotificationCenter(hwnd) || !GetWindowRect(hwnd, &rc) ||
        !CenterTarget(hwnd, region, rc, at) || (at.x == rc.left && at.y == rc.top)) {
        return;
    }
    Wh_Log(L"Notification center %ld,%ld -> %ld,%ld", rc.left, rc.top, at.x, at.y);
    g_placing = true;
    SetWindowPos(hwnd, nullptr, at.x, at.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    g_placing = false;
}

// Explorer's own move of the window, as the taskbar slides in or out: it goes where
// the mod puts it rather than back to the right end for a frame. Explorer starts
// placing the window from where this call asks.
bool PlaceCenterMove(HWND hwnd, int& x, int& y, int cx, int cy, UINT flags) {
    RECT now;
    if (g_placing || core::g_unloading || (flags & SWP_NOMOVE) || !IsNotificationCenter(hwnd) ||
        !GetWindowRect(hwnd, &now)) {
        return false;
    }
    int width = flags & SWP_NOSIZE ? now.right - now.left : cx;
    int height = flags & SWP_NOSIZE ? now.bottom - now.top : cy;
    RECT rc{x, y, x + width, y + height};
    POINT at;
    if (!CenterTarget(hwnd, nullptr, rc, at)) {
        return false;
    }
    if (at.x != x || at.y != y) {
        Wh_Log(L"Notification center move %d,%d -> %ld,%ld", x, y, at.x, at.y);
    }
    x = at.x;
    y = at.y;
    return true;
}

// Explorer uncloaks the window once it's in place; the move comes just before.
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd, DWORD attribute, LPCVOID value,
                                          DWORD size) {
    if (attribute == DWMWA_CLOAK && size == sizeof(BOOL) && value &&
        !*(const BOOL*)value && !core::g_unloading) {
        PlaceNotificationCenter(hwnd, nullptr);
    }
    return DwmSetWindowAttribute_Original(hwnd, attribute, value, size);
}

// Explorer cuts the window to the panel as it lays it out, and again whenever the
// panel's height changes (a notification comes or goes, the calendar folds): the
// panel's top stays under a top taskbar. The move comes before the new region.
int WINAPI SetWindowRgn_Hook(HWND hwnd, HRGN region, BOOL redraw) {
    if (region && !core::g_unloading) {
        PlaceNotificationCenter(hwnd, region);
    }
    return SetWindowRgn_Original(hwnd, region, redraw);
}

bool HookExplorer() {
    HMODULE dwm = LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto target = dwm ? (DwmSetWindowAttribute_t)GetProcAddress(dwm, "DwmSetWindowAttribute")
                      : nullptr;
    bool ok = target && WindhawkUtils::SetFunctionHook(target, DwmSetWindowAttribute_Hook,
                                                       &DwmSetWindowAttribute_Original);
    if (!ok) {
        Wh_Log(L"No DwmSetWindowAttribute hook: the notification center stays at the right");
    } else if (!WindhawkUtils::SetFunctionHook(SetWindowRgn, SetWindowRgn_Hook,
                                               &SetWindowRgn_Original)) {
        Wh_Log(L"No SetWindowRgn hook: under a top taskbar the panel keeps its first top");
    } else if (!WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                               &SetWindowPos_Original)) {
        Wh_Log(L"No SetWindowPos hook: explorer's own moves of the notification center show");
    }
    return ok;
}

// Taskbar thread: the spans go, and the notification center goes back where explorer
// had it. Asynchronous: its process may be suspended while the panel is closed.
void Restore() {
    for (auto& bar : g_bars) {
        if (bar.span && IsWindow(bar.hwnd)) {
            RemoveProp(bar.hwnd, kTrayProp);
        }
    }
    g_bars.clear();
    HWND center;
    POINT home, at;
    {
        std::lock_guard lock(g_centerLock);
        center = std::exchange(g_center, nullptr);
        home = g_centerDefault;
        at = g_centerAt;
    }
    RECT rc;
    if (!center || !IsWindow(center) || !GetWindowRect(center, &rc)) {
        return;
    }
    // Only a coordinate that still has the mod's value goes back.
    LONG x = rc.left == at.x ? home.x : rc.left;
    LONG y = rc.top == at.y ? home.y : rc.top;
    if (x != rc.left || y != rc.top) {
        SetWindowPos(center, nullptr, x, y, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

// ---- explorer: tray icon menus (SystemTray.dll, taskbar thread) ---------------------
// A tray icon's right-click menu (volume, network, the clock and bell, battery,
// language) is a XAML MenuFlyout. ContextMenuPositionHelper::GetShowPosition puts its
// right edge at the XAML root's width less a margin: the taskbar's right end, where
// the stock tray ends. The tray's own right edge takes the width's place. Icons in the
// overflow flyout are not under the tray frame and keep Windows' place.

using GetShowPosition_t = void*(WINAPI*)(void* point, int location, double margin,
                                          void* grid, double width, double height);
GetShowPosition_t GetShowPosition_Original;

void* WINAPI GetShowPosition_Hook(void* point, int location, double margin, void* grid,
                                  double width, double height) {
    // WindowsUdk TaskbarLocation: 1 top, 3 bottom.
    if ((location == 1 || location == 3) && grid && !core::g_unloading) {
        core::Guard(L"Tray menu", [&] {
            auto icon = core::FromAbi(*(void**)grid);
            auto tray = core::FindAncestor(icon, [](auto& p) {
                return core::IsClass(p, L"SystemTray.SystemTrayFrame");
            });
            if (tray && tray.ActualWidth() > 0) {
                auto r = tray.TransformToVisual(nullptr).TransformBounds(
                    {0, 0, (float)tray.ActualWidth(), (float)tray.ActualHeight()});
                double right = r.X + r.Width;
                if (right > 0 && right < width) {
                    width = right;
                }
            }
        });
    }
    return GetShowPosition_Original(point, location, margin, grid, width, height);
}

void CollectSystemTray(std::vector<WindhawkUtils::SYMBOL_HOOK>& list) {
    list.push_back(
        {{LR"(struct winrt::Windows::Foundation::Point __cdecl ContextMenuPositionHelper::GetShowPosition(enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,double,struct winrt::Windows::UI::Xaml::Controls::Grid const &,double,double))"},
         &GetShowPosition_Original, GetShowPosition_Hook, true});
}

// ---- ShellHost: Quick Settings -------------------------------------------------------

std::atomic<bool> g_on;  // the setting, in ShellHost

bool IsQuickSettings(HWND hwnd) {
    WCHAR cls[32];
    DWORD pid = 0;
    return GetWindowThreadProcessId(hwnd, &pid) && pid == GetCurrentProcessId() &&
           GetClassName(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"ControlCenterWindow") == 0;
}

// In ShellHost for Quick Settings, in explorer for the notification center.
BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy,
                              UINT flags) {
    constexpr UINT kStill = SWP_NOMOVE | SWP_NOSIZE;
    RECT rc;
    if (!PlaceCenterMove(hwnd, x, y, cx, cy, flags) && g_on && (flags & kStill) != kStill &&
        IsQuickSettings(hwnd) &&
        GetWindowRect(hwnd, &rc)) {
        int width = flags & SWP_NOSIZE ? rc.right - rc.left : cx;
        int height = flags & SWP_NOSIZE ? rc.bottom - rc.top : cy;
        if (flags & SWP_NOMOVE) {
            // A resize alone would keep the left edge; the right one stays at the tray.
            int placed = PlaceX(rc.left, rc.top, width, height);
            if (placed != rc.left) {
                flags &= ~SWP_NOMOVE;
                x = placed;
                y = rc.top;
            }
        } else {
            x = PlaceX(x, y, width, height);
        }
    }
    return SetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

bool InShellHost() {
    WCHAR path[MAX_PATH];
    DWORD n = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    return n && n < ARRAYSIZE(path) && HasName(path, L"ShellHost.exe");
}

// Quick Settings opens at the edge Windows' own setting names, read from the registry
// here. With the mod's edge set, that read gives the mod's edge, as the taskbars'
// reads in explorer do. -1: Windows' setting.
std::atomic<int> g_edge{-1};

int EdgeSetting() {
    switch (settings::g.pill.edge) {
        case settings::Edge::kTop:
            return ABE_TOP;
        case settings::Edge::kBottom:
            return ABE_BOTTOM;
        default:
            return -1;
    }
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY key, LPCWSTR subKey, LPCWSTR value, DWORD flags,
                              LPDWORD type, PVOID data, LPDWORD size) {
    DWORD room = size ? *size : 0;
    LONG ret = RegGetValueW_Original(key, subKey, value, flags, type, data, size);
    int edge = g_edge;
    if (edge < 0 || !value || !data || !size || room < sizeof(DWORD) ||
        !(flags & RRF_RT_REG_DWORD) || _wcsicmp(value, L"TaskbarLocation") != 0) {
        return ret;
    }
    *(DWORD*)data = (DWORD)edge;
    *size = sizeof(DWORD);
    if (type) {
        *type = REG_DWORD;
    }
    return ERROR_SUCCESS;
}

void LoadShellHostSettings() {
    g_on = settings::g.tray.panelsNearTray;
    g_edge = EdgeSetting();
}

// Wh_ModInit in ShellHost: only this part runs there.
bool InstallShellHost() {
    LoadShellHostSettings();
    auto regGetValue =
        (RegGetValueW_t)GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "RegGetValueW");
    if (!regGetValue || !WindhawkUtils::SetFunctionHook(regGetValue, RegGetValueW_Hook,
                                                        &RegGetValueW_Original)) {
        Wh_Log(L"No RegGetValueW hook: Quick Settings keeps Windows' edge");
    }
    return WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                          &SetWindowPos_Original);
}

}  // namespace panels

// ============================================================================
namespace center {
// ============================================================================
// The notification center (with the calendar) slides in from the right end of the
// screen, whatever edge the taskbar is on and wherever the panel opens. With
// tray.centerStill it appears in place. ShellExperienceHost.exe draws it, and its
// Windows.UI.ActionCenter.dll builds the slide on every show and hide from two
// expression literals (the second when it comes from the left). Explorer rewrites
// them in that process, the same length with "0 *" for "1 -", so the slide stays
// at 0 on the channel Windows animates anyway; nothing is loaded or hooked there.
// A literal read while half rewritten ("0 -", "1 *") is an expression all the
// same. Turning it off or unloading writes Windows' text back, and a new
// ShellExperienceHost starts with it.

constexpr WCHAR kImageTail[] = L"\\SystemApps\\ShellExperienceHost_cw5n1h2txyewy\\ShellExperienceHost.exe";
constexpr WCHAR kModule[] = L"Windows.UI.ActionCenter.dll";
// Windows' literal, then the still one.
constexpr std::wstring_view kSlides[][2] = {
    {L"AnimationProps.Width * (1 - AnimationProps.VisibilityProgress)",
     L"AnimationProps.Width * (0 * AnimationProps.VisibilityProgress)"},
    {L"-AnimationProps.Width * (1 - AnimationProps.VisibilityProgress)",
     L"-AnimationProps.Width * (0 * AnimationProps.VisibilityProgress)"},
};
constexpr DWORD kLookMs = 5000;  // while ShellExperienceHost or its module is not there

std::atomic<bool> g_on;  // tray.centerStill
HANDLE g_thread, g_stop, g_nudge;

// ShellExperienceHost as found, with the literals' addresses in it.
struct Target {
    HANDLE process = nullptr;
    DWORD pid = 0;
    BYTE* at[2] = {};
    void Close() {
        if (process) {
            CloseHandle(process);
        }
        *this = {};
    }
};

// This session's ShellExperienceHost, opened to read and write its memory; skip is
// one already given up on.
Target Open(DWORD skip) {
    DWORD session = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &session);
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return {};
    }
    Target t;
    PROCESSENTRY32W pe{.dwSize = sizeof(pe)};
    for (BOOL ok = Process32FirstW(snap, &pe); ok && !t.process; ok = Process32NextW(snap, &pe)) {
        DWORD s = 0;
        if (pe.th32ProcessID == skip || _wcsicmp(pe.szExeFile, L"ShellExperienceHost.exe") != 0 ||
            !ProcessIdToSessionId(pe.th32ProcessID, &s) || s != session) {
            continue;
        }
        HANDLE p = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ |
                                   PROCESS_VM_WRITE | PROCESS_VM_OPERATION,
                               FALSE, pe.th32ProcessID);
        WCHAR path[MAX_PATH];
        DWORD n = ARRAYSIZE(path);
        size_t tail = wcslen(kImageTail);
        if (p && QueryFullProcessImageNameW(p, 0, path, &n) && n >= tail &&
            _wcsicmp(path + n - tail, kImageTail) == 0) {
            t.process = p;
            t.pid = pe.th32ProcessID;
        } else if (p) {
            CloseHandle(p);
        }
    }
    CloseHandle(snap);
    return t;
}

bool Read(HANDLE process, const void* at, void* to, size_t size) {
    SIZE_T got = 0;
    return ReadProcessMemory(process, at, to, size, &got) && got == size;
}

// Where Windows.UI.ActionCenter.dll lies in the process; null until it is loaded.
BYTE* ModuleBase(DWORD pid) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (snap == INVALID_HANDLE_VALUE) {
        return nullptr;
    }
    BYTE* base = nullptr;
    MODULEENTRY32W me{.dwSize = sizeof(me)};
    for (BOOL ok = Module32FirstW(snap, &me); ok && !base; ok = Module32NextW(snap, &me)) {
        if (_wcsicmp(me.szModule, kModule) == 0) {
            base = me.modBaseAddr;
        }
    }
    CloseHandle(snap);
    return base;
}

// The literals in the module's read-only data, as whole strings (a terminator on
// both sides), in Windows' text or the still one. False when this build has
// neither.
bool FindLiterals(Target& t, BYTE* base) {
    IMAGE_DOS_HEADER dos;
    IMAGE_NT_HEADERS64 nt;
    if (!Read(t.process, base, &dos, sizeof(dos)) ||
        !Read(t.process, base + dos.e_lfanew, &nt, sizeof(nt))) {
        return false;
    }
    std::vector<IMAGE_SECTION_HEADER> sections(nt.FileHeader.NumberOfSections);
    BYTE* first = base + dos.e_lfanew + offsetof(IMAGE_NT_HEADERS64, OptionalHeader) +
                  nt.FileHeader.SizeOfOptionalHeader;
    if (sections.empty() ||
        !Read(t.process, first, sections.data(), sections.size() * sizeof(IMAGE_SECTION_HEADER))) {
        return false;
    }
    for (auto& section : sections) {
        if (memcmp(section.Name, ".rdata", 7) != 0) {
            continue;
        }
        std::vector<WCHAR> data(section.Misc.VirtualSize / sizeof(WCHAR));
        if (!Read(t.process, base + section.VirtualAddress, data.data(), data.size() * sizeof(WCHAR))) {
            return false;
        }
        for (int i = 0; i < 2; i++) {
            for (std::wstring_view text : kSlides[i]) {
                for (size_t j = 1; !t.at[i] && j + text.size() < data.size(); j++) {
                    if (data[j - 1] == L'\0' && data[j + text.size()] == L'\0' &&
                        std::wstring_view(&data[j], text.size()) == text) {
                        t.at[i] = base + section.VirtualAddress + j * sizeof(WCHAR);
                    }
                }
            }
        }
    }
    return t.at[0] && t.at[1];
}

// Both literals in Windows' text or the still one, each written only when it is
// not that already. False when the memory could not be read or written.
bool Set(Target& t, bool still) {
    for (int i = 0; i < 2; i++) {
        std::wstring_view want = kSlides[i][still];
        std::wstring now(want.size(), L'\0');
        if (!Read(t.process, t.at[i], now.data(), want.size() * sizeof(WCHAR))) {
            return false;
        }
        if (now == want) {
            continue;
        }
        SIZE_T size = want.size() * sizeof(WCHAR), done = 0;
        DWORD old;
        if (!VirtualProtectEx(t.process, t.at[i], size, PAGE_READWRITE, &old)) {
            return false;
        }
        bool ok = WriteProcessMemory(t.process, t.at[i], want.data(), size, &done) && done == size;
        VirtualProtectEx(t.process, t.at[i], size, old, &old);
        if (!ok) {
            return false;
        }
    }
    return true;
}

// Windhawk loads the mod into every explorer.exe; the one with the taskbar does this.
bool OwnsTaskbar() {
    DWORD pid = 0;
    HWND bar = FindWindowW(L"Shell_TrayWnd", nullptr);
    return bar && GetWindowThreadProcessId(bar, &pid) && pid == GetCurrentProcessId();
}

DWORD WINAPI WatchProc(void*) {
    Target t;
    DWORD skip = 0;  // a process the literals could not be found or set in
    bool logged = false;
    int state = -1;  // the text the literals have, as far as this load knows
    for (;;) {
        bool on = g_on;
        if (!t.process && OwnsTaskbar()) {
            t = Open(skip);
            BYTE* base = t.process ? ModuleBase(t.pid) : nullptr;
            if (t.process && !base) {
                t.Close();  // its module comes with the first show
            } else if (t.process && !FindLiterals(t, base)) {
                Wh_Log(L"Notification center slide not found in %u; left as it is", t.pid);
                skip = t.pid;
                t.Close();
            }
            state = -1;
        }
        if (t.process && state != (int)on) {
            if (Set(t, on)) {
                state = on;
                if (on || logged) {
                    Wh_Log(L"Notification center slide %s in %u", on ? L"off" : L"back", t.pid);
                    logged = true;
                }
            } else {
                Wh_Log(L"Notification center slide could not be set in %u (%u)", t.pid, GetLastError());
                skip = t.pid;
                t.Close();
            }
        }
        HANDLE waits[3] = {g_stop, g_nudge, t.process};
        DWORD r = WaitForMultipleObjects(t.process ? 3 : 2, waits, FALSE, t.process ? INFINITE : kLookMs);
        if (r == WAIT_OBJECT_0) {
            break;
        }
        if (r == WAIT_OBJECT_0 + 2) {
            t.Close();  // it ended; a new one starts with Windows' text
        }
    }
    if (t.process && state == 1 && !Set(t, false)) {
        Wh_Log(L"Notification center slide could not be put back in %u", t.pid);
    }
    t.Close();
    return 0;
}

void LoadSettings() {
    g_on = settings::g.tray.centerStill;
    if (g_nudge) {
        SetEvent(g_nudge);
    }
}

// Wh_ModAfterInit, in explorer.
void Start() {
    LoadSettings();
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_nudge = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_thread = g_stop && g_nudge ? CreateThread(nullptr, 0, WatchProc, nullptr, 0, nullptr) : nullptr;
    if (!g_thread) {
        Wh_Log(L"Notification center watch did not start");
    }
}

// Wh_ModBeforeUninit: Windows' text goes back before the module does.
void Stop() {
    if (g_thread) {
        SetEvent(g_stop);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
    }
    for (HANDLE* h : {&g_stop, &g_nudge}) {
        if (*h) {
            CloseHandle(*h);
        }
    }
    g_thread = g_stop = g_nudge = nullptr;
}

}  // namespace center

// ============================================================================
namespace position {
// ============================================================================
// The taskbar at the top or bottom edge whatever Windows' setting says, through
// the location Windows has of its own on newer builds (native): each taskbar
// reads the location from the registry as it sets itself up, and its content
// asks TaskbarSettings::Location. Both get the set edge. Nothing is written, so
// turning this off or unloading puts the taskbar back where Windows' setting
// has it. A build without the native location is left alone.

using SH = WindhawkUtils::SYMBOL_HOOK;

constexpr WCHAR kAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr WCHAR kLocationValue[] = L"TaskbarLocation";

bool g_native;  // set while the mod initializes
// True on a thread while a taskbar reads its own location there.
thread_local bool g_reading;
// The edge the taskbars were told, -1 if none: unloading tells them Windows' again.
std::atomic<int> g_told{-1};

bool Active() {
    return g_native && !core::g_unloading &&
           settings::g.pill.edge != settings::Edge::kWindows;
}

// The TaskbarLocation values are the screen edges (ABE_*).
int SetEdge() {
    return settings::g.pill.edge == settings::Edge::kTop ? ABE_TOP : ABE_BOTTOM;
}

int ForcedEdge() {
    return Active() ? SetEdge() : -1;
}

// Windows' own setting.
int StoredEdge() {
    DWORD edge = ABE_BOTTOM, size = sizeof(edge);
    if (RegGetValueW(HKEY_CURRENT_USER, kAdvancedKey, kLocationValue, RRF_RT_REG_DWORD,
                     nullptr, &edge, &size) != ERROR_SUCCESS ||
        edge > ABE_BOTTOM) {
        edge = ABE_BOTTOM;
    }
    return (int)edge;
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY key,
                              LPCWSTR subKey,
                              LPCWSTR value,
                              DWORD flags,
                              LPDWORD type,
                              PVOID data,
                              LPDWORD size) {
    DWORD room = size ? *size : 0;
    LONG ret = RegGetValueW_Original(key, subKey, value, flags, type, data, size);
    if (!g_reading || key != HKEY_CURRENT_USER || !subKey || !value || !data ||
        !size || !(flags & RRF_RT_REG_DWORD) || room < sizeof(DWORD) ||
        _wcsicmp(subKey, kAdvancedKey) != 0 || _wcsicmp(value, kLocationValue) != 0 ||
        !Active()) {
        return ret;
    }
    *(DWORD*)data = (DWORD)SetEdge();
    *size = sizeof(DWORD);
    if (type) {
        *type = REG_DWORD;
    }
    return ERROR_SUCCESS;
}

using TrayUI__GetSaveStateAndInitRects_t = void(WINAPI*)(void* pThis);
TrayUI__GetSaveStateAndInitRects_t TrayUI__GetSaveStateAndInitRects_Original;
void WINAPI TrayUI__GetSaveStateAndInitRects_Hook(void* pThis) {
    bool was = std::exchange(g_reading, true);
    TrayUI__GetSaveStateAndInitRects_Original(pThis);
    g_reading = was;
}

using CSecondaryTray__LoadSettings_t = HRESULT(WINAPI*)(void* pThis);
CSecondaryTray__LoadSettings_t CSecondaryTray__LoadSettings_Original;
HRESULT WINAPI CSecondaryTray__LoadSettings_Hook(void* pThis) {
    bool was = std::exchange(g_reading, true);
    HRESULT ret = CSecondaryTray__LoadSettings_Original(pThis);
    g_reading = was;
    return ret;
}

using TaskbarSettings_Location_t = int(WINAPI*)(void* pThis);
TaskbarSettings_Location_t TaskbarSettings_Location_Original;
int WINAPI TaskbarSettings_Location_Hook(void* pThis) {
    return Active() ? SetEdge() : TaskbarSettings_Location_Original(pThis);
}

// wil's state of the native location feature, and its check.
void* g_feature;
using FeatureIsEnabled_t = bool(WINAPI*)(void* pThis, int reportingKind);
FeatureIsEnabled_t g_featureIsEnabled;
// Overloads only builds from before the native location have.
void* g_oldVerifySize;
void* g_oldHandleSizing;

void CollectTaskbarDll(std::vector<SH>& list) {
    list.push_back({{LR"(class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768> `private: static class wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768> & __cdecl wil::Feature<struct __WilFeatureTraits_Feature_59213768>::GetImpl(void)'::`2'::impl)",
                     LR"(class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768> `private: static class wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768> & __cdecl wil::Feature<struct __WilExternalFeatureTraits_Feature_59213768>::GetImpl(void)'::`2'::impl)"},
                    &g_feature, nullptr, true});
    list.push_back({{LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilFeatureTraits_Feature_59213768>::__private_IsEnabled(enum wil::ReportingKind))",
                     LR"(public: bool __cdecl wil::details::FeatureImpl<struct __WilExternalFeatureTraits_Feature_59213768>::__private_IsEnabled(enum wil::ReportingKind))"},
                    &g_featureIsEnabled, nullptr, true});
    list.push_back({{LR"(public: virtual void __cdecl TrayUI::VerifySize(bool,bool))"},
                    &g_oldVerifySize, nullptr, true});
    list.push_back({{LR"(public: int __cdecl TrayUI::_HandleSizing(unsigned __int64,struct tagRECT *,unsigned int,bool))"},
                    &g_oldHandleSizing, nullptr, true});
    list.push_back({{LR"(public: void __cdecl TrayUI::_GetSaveStateAndInitRects(void))"},
                    &TrayUI__GetSaveStateAndInitRects_Original,
                    TrayUI__GetSaveStateAndInitRects_Hook, true});
    list.push_back({{LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"},
                    &CSecondaryTray__LoadSettings_Original,
                    CSecondaryTray__LoadSettings_Hook, true});
    list.push_back({{LR"(public: enum winrt::WindowsUdk::UI::Shell::TaskbarLocation __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings::Location(void))"},
                    &TaskbarSettings_Location_Original, TaskbarSettings_Location_Hook,
                    true});
}

// After taskbar.dll is hooked, in Wh_ModInit: whether this build has the
// location of its own, and if so the registry read is caught too.
void TaskbarDllHooked() {
    bool native;
    if (g_feature && g_featureIsEnabled) {
        constexpr int kReportingKindNone = 0;
        native = g_featureIsEnabled(g_feature, kReportingKindNone);
    } else {
        // The native location gave these a monitor parameter.
        native = !g_oldVerifySize && !g_oldHandleSizing;
    }
    g_native = native && TrayUI__GetSaveStateAndInitRects_Original &&
               TaskbarSettings_Location_Original;
    Wh_Log(L"Taskbar location: native %d (flag %d), primary %d, secondary %d, content %d",
           native, g_feature && g_featureIsEnabled,
           TrayUI__GetSaveStateAndInitRects_Original != nullptr,
           CSecondaryTray__LoadSettings_Original != nullptr,
           TaskbarSettings_Location_Original != nullptr);
    if (!g_native) {
        return;
    }
    auto regGetValue = (RegGetValueW_t)GetProcAddress(GetModuleHandle(L"kernelbase.dll"),
                                                      "RegGetValueW");
    if (!regGetValue ||
        !WindhawkUtils::SetFunctionHook(regGetValue, RegGetValueW_Hook,
                                        &RegGetValueW_Original)) {
        Wh_Log(L"RegGetValueW not hooked");
        g_native = false;
        return;
    }
    core::g_forcedEdge = ForcedEdge;
}

// Moves the taskbars to the set edge, or back to Windows' one once unloading.
void Apply() {
    if (!g_native) {
        return;
    }
    int edge = ForcedEdge();
    if (edge < 0) {
        if (g_told.exchange(-1) < 0) {
            return;
        }
        edge = StoredEdge();
    } else if (g_told.exchange(edge) == edge) {
        return;
    }
    HWND taskbar = core::PrimaryTaskbar();
    if (!taskbar) {
        return;
    }
    Wh_Log(L"Taskbar to edge %d", edge);
    // The primary moves with the change the Settings app sends, which has its
    // content read the location again too; the secondaries read theirs again.
    SendMessage(taskbar, core::kSettingChangeMsg, core::kLocationSetting, edge);
    SendMessage(taskbar, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

}  // namespace position

namespace entry {
void ApplyNew();
}

// ============================================================================
namespace hooks {
// ============================================================================

struct Features {
    bool pill;
    bool tray;
    bool size;
    bool autohide;
    bool media;
    bool usage;
    bool island;
    bool folders;
    bool start;
    bool panels;
    bool position;
    bool operator==(const Features&) const = default;
};

Features g_features;

Features Wanted() {
    // The tray part always has the clock to write in its 24 or 12 hours.
    return {.pill = settings::g.pill.enabled, .tray = true,
            .size = settings::g.size.enabled,
            .autohide = settings::g.autohide.enabled,
            .media = settings::g.media.enabled,
            .usage = settings::g.usage.enabled,
            // It brings the taskbar out through the auto-hide hooks.
            .island = settings::g.island.enabled && settings::g.autohide.enabled,
            .folders = settings::g.folders.enabled,
            .start = settings::g.start.mode != settings::StartMode::kWindows,
            // The stock tray is at the right end already.
            .panels = settings::g.pill.enabled && settings::g.tray.panelsNearTray,
            .position = settings::g.pill.edge != settings::Edge::kWindows};
}

// ---- shared hooks -------------------------------------------------------------

using OnApplyTemplate_t = int(WINAPI*)(void* pThis);

// The features that keep something of their own on every taskbar.
bool PerTaskbar() {
    return g_features.media || g_features.usage || g_features.island ||
           g_features.start;
}

[[clang::no_destroy]] std::list<FrameworkElement::Loaded_revoker> g_frameLoads;

// A new taskbar (a monitor came) gets those once its frame is in the tree,
// when its window can find it.
void OnFrameTemplate(FrameworkElement const& frame) {
    auto it = g_frameLoads.emplace(g_frameLoads.end());
    *it = frame.Loaded(winrt::auto_revoke, [it](auto&&, auto&&) {
        g_frameLoads.erase(it);  // destroys this handler; locals only below
        if (!core::g_unloading) {
            core::Post(entry::ApplyNew);
        }
    });
}

OnApplyTemplate_t TaskbarFrame_OnApplyTemplate_Original;
int WINAPI TaskbarFrame_OnApplyTemplate_Hook(void* pThis) {
    int ret = TaskbarFrame_OnApplyTemplate_Original(pThis);
    if (!core::g_unloading) {
        core::Guard(L"TaskbarFrame template", [&] {
            core::NoteUiThread();
            if (auto frame = core::FromAbi(pThis)) {
                if (g_features.pill) {
                    pill::OnFrameTemplate(frame);
                }
                if (PerTaskbar()) {
                    OnFrameTemplate(frame);
                }
            }
        });
    }
    return ret;
}

OnApplyTemplate_t TaskbarBackground_OnApplyTemplate_Original;
int WINAPI TaskbarBackground_OnApplyTemplate_Hook(void* pThis) {
    int ret = TaskbarBackground_OnApplyTemplate_Original(pThis);
    if (!core::g_unloading && !core::g_vertical) {
        core::Guard(L"Background template", [&] {
            // Thumbnail previews use the same control, in another window or a
            // popup; only the one in a taskbar's own tree is the bar.
            auto bg = core::FromAbi(pThis);
            auto root = bg ? bg.XamlRoot() : nullptr;
            auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
            bool bar = content &&
                       std::ranges::any_of(core::Taskbars(),
                                           [&](HWND hwnd) {
                                               return core::TaskbarRoot(hwnd) == root;
                                           }) &&
                       core::FindAncestor(bg, [&](auto& p) { return p == content; });
            if (bar) {
                pill::StyleBackground(bg);
            }
        });
    }
    return ret;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    if (g_features.size) {
        size::UpdateVisualStates(pThis, TaskListButton_UpdateVisualStates_Original);
    } else {
        TaskListButton_UpdateVisualStates_Original(pThis);
    }
    if (!core::g_unloading && g_features.pill) {
        core::Guard(L"Task button state", [&] {
            if (auto button = core::FromAbi((IUnknown*)pThis + 3)) {
                pill::StyleTaskButton(button);
            }
        });
    }
    if (!core::g_unloading && g_features.folders) {
        core::Guard(L"Folder button state", [&] {
            if (auto button = core::FromAbi((IUnknown*)pThis + 3)) {
                folders::HideRunningLine(button);
            }
        });
    }
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
[[clang::no_destroy]] std::list<FrameworkElement::Loaded_revoker> g_iconLoads;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);
    if (core::g_unloading) {
        return ret;
    }
    core::Guard(L"Icon view", [&] {
        auto icon = core::FromAbi(((IUnknown**)pThis)[1]);
        if (!icon) {
            return;
        }
        core::NoteUiThread();
        auto it = g_iconLoads.emplace(g_iconLoads.end());
        *it = icon.Loaded(winrt::auto_revoke, [it](auto&& sender, auto&&) {
            auto icon = sender.template try_as<FrameworkElement>();
            g_iconLoads.erase(it);  // destroys this handler; locals only below
            if (!icon || core::g_unloading) {
                return;
            }
            if (g_features.pill) {
                core::Guard(L"Icon loaded", [&] { pill::OnIconLoaded(icon); });
            }
            if (g_features.tray) {
                core::Guard(L"Tray icon loaded", [&] { tray::OnIcon(icon); });
            }
            if (g_features.media || g_features.usage) {
                core::Guard(L"Widgets placed", widgets::PlaceWaiting);
            }
        });
    });
    return ret;
}

// ---- registration ---------------------------------------------------------------
// Only the symbols of enabled features are hooked; a feature set change
// reloads the mod (Wh_ModSettingsChanged).

using SH = WindhawkUtils::SYMBOL_HOOK;

std::vector<SH> g_taskbarViewHooks;
std::vector<SH> g_systemTrayHooks;
std::atomic<bool> g_taskbarViewHooked;
std::atomic<bool> g_systemTrayHooked;

bool HookTaskbarDll() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    std::vector<SH> hooks;
    hooks.push_back({{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
                     &core::CTaskBand_ITaskListWndSite_vftable});
    hooks.push_back({{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
                     &core::CSecondaryTaskBand_ITaskListWndSite_vftable});
    hooks.push_back({{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
                     &core::CTaskBand_GetTaskbarHost_Original});
    hooks.push_back({{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
                     &core::CSecondaryTaskBand_GetTaskbarHost_Original});
    hooks.push_back({{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
                     &core::TaskbarHost_FrameHeight_Original});
    hooks.push_back({{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
                     &core::std__Ref_count_base__Decref_Original});
    if (g_features.size) {
        size::CollectTaskbarDll(hooks);
    }
    if (g_features.autohide) {
        autohide::CollectTaskbarDll(hooks);
    }
    if (g_features.folders) {
        folders::CollectTaskbarDll(hooks);
    }
    if (g_features.position) {
        position::CollectTaskbarDll(hooks);
    }
    bool ok = WindhawkUtils::HookSymbols(module, hooks.data(), hooks.size());
    if (ok && g_features.folders) {
        folders::TaskbarDllHooked();
    }
    if (ok && g_features.position) {
        position::TaskbarDllHooked();
    }
    return ok;
}

void Collect() {
    if (g_features.pill || PerTaskbar()) {
        g_taskbarViewHooks.push_back(
            {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
             &TaskbarFrame_OnApplyTemplate_Original,
             TaskbarFrame_OnApplyTemplate_Hook});
    }
    if (g_features.pill) {
        g_taskbarViewHooks.push_back(
            {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarBackground,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
             &TaskbarBackground_OnApplyTemplate_Original,
             TaskbarBackground_OnApplyTemplate_Hook});
    }
    if (g_features.pill || g_features.size || g_features.folders) {
        g_taskbarViewHooks.push_back(
            {{LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
             &TaskListButton_UpdateVisualStates_Original,
             TaskListButton_UpdateVisualStates_Hook,
             !(g_features.pill || g_features.size)});  // the folders need it only for the running line
    }
    if (g_features.size) {
        size::CollectTaskbarView(g_taskbarViewHooks);
        size::CollectSystemTray(g_systemTrayHooks);
    }
    if (g_features.size || g_features.start) {
        size::CollectToggleButton(g_taskbarViewHooks);
    }
    if (g_features.autohide) {
        autohide::CollectTaskbarView(g_taskbarViewHooks);
    }
    if (g_features.folders) {
        folders::CollectTaskbarView(g_taskbarViewHooks);
    }
    if (g_features.pill || g_features.tray) {
        g_systemTrayHooks.push_back(
            {{LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
             &IconView_IconView_Original, IconView_IconView_Hook});
    }
    if (g_features.panels) {
        panels::CollectSystemTray(g_systemTrayHooks);
    }
}

// Hooks one module's list once it's loaded; true if it was hooked now.
bool HookModule(PCWSTR name,
                std::vector<SH>& list,
                std::atomic<bool>& hooked,
                HMODULE loaded,
                void (*after)()) {
    if (list.empty() || hooked) {
        return false;
    }
    HMODULE module = GetModuleHandle(name);
    if (!module || (loaded && module != loaded) || hooked.exchange(true)) {
        return false;
    }
    Wh_Log(L"Hooking %s (%zu)", name, list.size());
    if (!WindhawkUtils::HookSymbols(module, list.data(), list.size())) {
        Wh_Log(L"HookSymbols failed for %s", name);
        return false;
    }
    after();
    return true;
}

void TaskbarViewHooked() {
    if (g_features.size) {
        size::TaskbarViewHooked();
    }
    if (g_features.folders) {
        folders::TaskbarViewHooked();
    }
}

void SystemTrayHooked() {
    if (g_features.size) {
        size::SystemTrayHooked();
    }
}

bool HookLoadedModules(HMODULE loaded) {
    bool any = HookModule(L"Taskbar.View.dll", g_taskbarViewHooks,
                          g_taskbarViewHooked, loaded, TaskbarViewHooked);
    any |= HookModule(L"SystemTray.dll", g_systemTrayHooks, g_systemTrayHooked,
                      loaded, SystemTrayHooked);
    return any;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && HookLoadedModules(module)) {
        Wh_ApplyHookOperations();
    }
    return module;
}

bool Install() {
    g_features = Wanted();
    Collect();
    if (g_features == Features{}) {
        return true;
    }
    if (!HookTaskbarDll()) {
        return false;
    }
    if (g_features.size) {
        size::InstallWin32();
    }
    if (g_features.panels) {
        panels::HookExplorer();
    }
    // Without its launch hook a folder's pin would open the folder in File Explorer.
    if (g_features.folders && !folders::Install()) {
        return false;
    }
    HookLoadedModules(nullptr);
    bool pending = (!g_taskbarViewHooks.empty() && !g_taskbarViewHooked) ||
                   (!g_systemTrayHooks.empty() && !g_systemTrayHooked);
    if (pending) {
        auto loadLibrary = (LoadLibraryExW_t)GetProcAddress(
            GetModuleHandle(L"kernelbase.dll"), "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(loadLibrary, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }
    return true;
}

// In case a module loaded while Wh_ModInit ran.
void AfterInit() {
    if (HookLoadedModules(nullptr)) {
        Wh_ApplyHookOperations();
    }
}

}  // namespace hooks

// ============================================================================
namespace bloom_link {
// ============================================================================
// Bloom's taskbar page. Where Bloom is installed, the mod writes the pins and
// folders it sees to %LOCALAPPDATA%\Bloom\addons\pill-taskbar\state.json and
// carries out the folder commands Bloom drops into inbox\, one at a time in
// name order, with each result in outbox\. Without a Bloom folder nothing is
// created. One thread waits on the inbox folder, the taskbar's pin list in the
// registry and nudges from the folder code; the shell work runs on folder jobs,
// one after the other.

using Microsoft::WRL::ComPtr;
namespace wdj = winrt::Windows::Data::Json;

constexpr wchar_t kAddonsDir[] = L"addons";
constexpr wchar_t kDataDir[] = L"pill-taskbar";
constexpr wchar_t kStateFile[] = L"state.json";
constexpr wchar_t kInboxDir[] = L"inbox";
constexpr wchar_t kOutboxDir[] = L"outbox";
constexpr wchar_t kIconsDir[] = L".icons";  // in .pins: icon files Bloom handed over
constexpr wchar_t kTaskbandKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband";
constexpr int kStateVersion = 1;
constexpr DWORD kMaxCommandBytes = 64 * 1024;
constexpr DWORD kMaxIconBytes = 4 * 1024 * 1024;
constexpr size_t kMaxIdLength = 100;
constexpr size_t kMaxNameLength = 120;  // a folder's name
constexpr size_t kMaxText = 2048;       // any other string in a command
constexpr size_t kMaxApps = 64;         // create's apps
constexpr size_t kMaxNames = 512;       // order's names
constexpr DWORD kSettleMs = 300;        // nudges within this make one state write
constexpr DWORD kRetryMs = 200;         // a command file still being written ...
constexpr int kMaxTries = 10;           // ... is given up after this many reads
constexpr DWORD kJobDrainMs = 5000;     // unload waits this long for a job under way
constexpr DWORD kTaskbarWaitMs = 1000;
constexpr int kTaskbarWaits = 120;

// Result errors, as Bloom knows them.
constexpr wchar_t kFoldersOff[] = L"foldersOff";
constexpr wchar_t kNoFolder[] = L"noFolder";
constexpr wchar_t kExists[] = L"exists";
constexpr wchar_t kBadName[] = L"badName";
constexpr wchar_t kNotFound[] = L"notFound";
constexpr wchar_t kBadArgs[] = L"badArgs";
constexpr wchar_t kFailed[] = L"failed";

// ---- JSON text and names (no shell) -------------------------------------------

// A JSON string literal.
std::wstring Quote(std::wstring_view text) {
    std::wstring out = L"\"";
    for (wchar_t c : text) {
        if (c == L'"' || c == L'\\') {
            out += L'\\';
            out += c;
        } else if (c < 0x20) {
            out += L"\\u00";
            out += L"0123456789abcdef"[c >> 4];
            out += L"0123456789abcdef"[c & 15];
        } else {
            out += c;
        }
    }
    return out + L"\"";
}

std::string Utf8(std::wstring_view text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0,
                                nullptr, nullptr);
    std::string out(n > 0 ? n : 0, '\0');
    if (n > 0)
        WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), out.data(), n,
                            nullptr, nullptr);
    return out;
}

// UTC, to the second: YYYY-MM-DDThh:mm:ssZ.
std::wstring IsoTime(const SYSTEMTIME& t) {
    auto two = [](int n) { return std::wstring{wchar_t(L'0' + n / 10 % 10), wchar_t(L'0' + n % 10)}; };
    return std::to_wstring(t.wYear) + L"-" + two(t.wMonth) + L"-" + two(t.wDay) + L"T" +
           two(t.wHour) + L":" + two(t.wMinute) + L":" + two(t.wSecond) + L"Z";
}

// FNV-1a over a file's bytes, as folders::HashHex does over text.
std::wstring HashBytes(std::string_view bytes) {
    uint64_t hash = 14695981039346656037ull;
    for (char c : bytes) hash = (hash ^ (unsigned char)c) * 1099511628211ull;
    std::wstring hex(16, L'0');
    for (int i = 15; i >= 0; i--, hash >>= 4) hex[i] = L"0123456789abcdef"[hash & 15];
    return hex;
}

// CON, NUL, COM1 and the like name a device, with any extension.
bool IsReservedName(std::wstring_view name) {
    std::wstring stem(name.substr(0, name.find(L'.')));
    while (!stem.empty() && stem.back() == L' ') stem.pop_back();
    for (wchar_t& c : stem) c = towupper(c);
    for (PCWSTR device : {L"CON", L"PRN", L"AUX", L"NUL"})
        if (stem == device) return true;
    return stem.size() == 4 && (stem.starts_with(L"COM") || stem.starts_with(L"LPT")) &&
           stem[3] >= L'0' && stem[3] <= L'9';
}

// One file or folder name: no path, no control characters, not hidden by a leading dot.
bool IsPlainName(std::wstring_view name, size_t maxLength) {
    return !name.empty() && name.size() <= maxLength && name[0] != L'.' &&
           name.find_first_of(folders::kInvalidNameChars) == std::wstring_view::npos &&
           std::none_of(name.begin(), name.end(), [](wchar_t c) { return c < 0x20; });
}

bool IsFolderName(const std::wstring& name) {
    return folders::IsValidFolderName(name) && IsPlainName(name, kMaxNameLength) &&
           !IsReservedName(name);
}

// `<id>.json`, where the id is letters, digits, '-', '_' and '.'.
bool IsCommandFile(std::wstring_view name) {
    constexpr std::wstring_view kExtension = L".json";
    if (name.size() <= kExtension.size() || name.size() > kMaxIdLength + kExtension.size() ||
        _wcsnicmp(name.data() + name.size() - kExtension.size(), kExtension.data(),
                  kExtension.size()) != 0 ||
        name[0] == L'.')
        return false;
    return std::all_of(name.begin(), name.end() - kExtension.size(), [](wchar_t c) {
        return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') ||
               c == L'-' || c == L'_' || c == L'.';
    });
}

// ---- paths and files ----------------------------------------------------------

std::wstring g_bloom, g_data, g_inbox, g_outbox;  // set before the thread starts

bool InitPaths() {
    WCHAR base[MAX_PATH];
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", base, ARRAYSIZE(base));
    if (!n || n >= ARRAYSIZE(base)) return false;
    g_bloom = std::wstring(base) + L"\\" + usage::kBloomDirName;
    g_data = g_bloom + L"\\" + kAddonsDir + L"\\" + kDataDir;
    g_inbox = g_data + L"\\" + kInboxDir;
    g_outbox = g_data + L"\\" + kOutboxDir;
    return usage::IsDirectory(g_bloom);
}

// The data folders, only while Bloom's own is there.
bool MakeFolders() {
    if (!usage::IsDirectory(g_bloom)) return false;
    for (const std::wstring& dir : {g_bloom + L"\\" + kAddonsDir, g_data, g_inbox, g_outbox})
        CreateDirectoryW(dir.c_str(), nullptr);
    return usage::IsDirectory(g_data);
}

// Through a hidden temporary file, so a reader never sees half of it.
bool WriteReplacing(const std::wstring& dir, const std::wstring& fileName, const std::string& bytes) {
    std::wstring path = dir + L"\\" + fileName, temp = dir + L"\\." + fileName + L".tmp";
    if (folders::WriteFileBytes(temp, {bytes.begin(), bytes.end()}) &&
        MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING))
        return true;
    DeleteFileW(temp.c_str());
    return false;
}

enum class Got { kOk, kGone, kBusy, kTooBig };

// Without write sharing: a file Bloom still has open for writing is busy.
Got ReadSmallFile(const std::wstring& path, DWORD maxBytes, std::string* bytes) {
    winrt::file_handle file{CreateFileW(path.c_str(), GENERIC_READ,
                                        FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                                        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)};
    if (!file) {
        DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND ? Got::kGone
                                                                              : Got::kBusy;
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file.get(), &size)) return Got::kBusy;
    if (size.QuadPart > maxBytes) return Got::kTooBig;
    bytes->resize((size_t)size.QuadPart);
    DWORD got = 0;
    if (!bytes->empty() &&
        !ReadFile(file.get(), bytes->data(), (DWORD)bytes->size(), &got, nullptr))
        return Got::kBusy;
    bytes->resize(got);
    return Got::kOk;
}

// ---- state.json (folder jobs) -------------------------------------------------

std::wstring AppFields(const std::wstring& name, const std::wstring& path, const std::wstring& appId) {
    return L"\"name\":" + Quote(name) + L",\"path\":" + Quote(path) + L",\"appId\":" + Quote(appId);
}

std::wstring DisplayName(PCIDLIST_ABSOLUTE pidl) {
    PWSTR name = nullptr;
    std::wstring text;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &name)) && name) {
        text = name;
        CoTaskMemFree(name);
    }
    return text;
}

std::wstring FolderState(const std::wstring& folder) {
    folders::IconLocation icon = folders::PinIcon(folder);
    std::wstring out = folders::IsPreviewIcon(icon)
                           ? L"{\"icon\":{\"kind\":\"auto\"}"
                           : L"{\"icon\":{\"kind\":\"file\",\"path\":" + Quote(icon.path) +
                                 L",\"index\":" + std::to_wstring(icon.index) + L"}";
    out += L",\"apps\":[";
    bool first = true;
    for (const folders::MenuItem& item : folders::LoadItems(folder, 0)) {
        out += first ? L"{" : L",{";
        first = false;
        out += L"\"file\":" + Quote(item.fileName) + L"," +
               AppFields(item.label, item.path, folders::ItemAppId(folder, item.fileName)) + L"}";
    }
    return out + L"]}";
}

// The loose folder's apps, once each, in the order they came.
std::wstring RunningApps() {
    std::vector<folders::LooseWindow> apps;
    AcquireSRWLockShared(&folders::g_lock);
    for (const auto& [window, loose] : folders::g_running.loose) {
        if (!IsWindow(window) || !IsWindowVisible(window)) continue;
        auto it = std::find_if(apps.begin(), apps.end(),
                               [&](const folders::LooseWindow& app) { return app.key == loose.key; });
        if (it == apps.end())
            apps.push_back(loose);
        else if (loose.order < it->order)
            *it = loose;
    }
    ReleaseSRWLockShared(&folders::g_lock);
    std::sort(apps.begin(), apps.end(), [](const folders::LooseWindow& a, const folders::LooseWindow& b) {
        return a.order < b.order;
    });
    std::wstring out;
    for (const folders::LooseWindow& app : apps) {
        ComPtr<IShellItem> shell = folders::LooseShellItem(app);
        std::wstring name = shell ? folders::AppLabel(shell.Get()) : L"";
        out += (out.empty() ? L"{" : L",{") +
               AppFields(name.empty() ? app.appId : name, app.path, app.appId) + L"}";
    }
    return out;
}

// Everything but the header, which changes with every write.
std::wstring StateBody() {
    std::wstring root = hooks::g_features.folders ? folders::Root() : L"";
    AcquireSRWLockShared(&folders::g_lock);
    bool looseOn = folders::g_looseOn && !root.empty();
    std::wstring looseName = folders::g_looseName;
    ReleaseSRWLockShared(&folders::g_lock);

    std::wstring pinned, items;
    std::vector<std::wstring> seen;
    for (const folders::Pin& pin : folders::ReadPins()) {
        std::wstring folder = root.empty() ? L"" : folders::FolderOfPin(pin);
        if (!pinned.empty()) pinned += L",";
        if (folder.empty()) {
            pinned += L"{\"kind\":\"app\"," +
                      AppFields(DisplayName(pin.pidl.get()), folders::PathFromPidl(pin.pidl.get()),
                                pin.appId) +
                      L"}";
            continue;
        }
        std::wstring name = folders::FolderName(folder);
        pinned += L"{\"kind\":\"folder\",\"name\":" + Quote(name) + L"}";
        if (std::find(seen.begin(), seen.end(), name) != seen.end()) continue;
        seen.push_back(name);
        items += (items.empty() ? L"" : L",") + Quote(name) + L":" + FolderState(folder);
    }
    if (looseOn) pinned += (pinned.empty() ? L"" : L",") + std::wstring(L"{\"kind\":\"running\"}");
    return L"\"folders\":{\"enabled\":" + std::wstring(root.empty() ? L"false" : L"true") +
           L",\"path\":" + Quote(root) + L"},\"pinned\":[" + pinned + L"],\"items\":{" + items +
           L"},\"running\":{\"enabled\":" + (looseOn ? L"true" : L"false") + L",\"name\":" +
           Quote(looseName) + L",\"apps\":[" + (looseOn ? RunningApps() : L"") + L"]}";
}

std::wstring g_lastBody;  // jobs run one at a time

// Written only when something in it changed, or when the file went missing.
void WriteState() {
    if (!MakeFolders()) return;
    std::wstring body = StateBody();
    if (body == g_lastBody &&
        GetFileAttributesW((g_data + L"\\" + kStateFile).c_str()) != INVALID_FILE_ATTRIBUTES)
        return;
    SYSTEMTIME now;
    GetSystemTime(&now);
    std::wstring json = L"{\"v\":" + std::to_wstring(kStateVersion) + L",\"pid\":" +
                        std::to_wstring(GetCurrentProcessId()) + L",\"writtenAt\":" +
                        Quote(IsoTime(now)) + L"," + body + L"}";
    if (WriteReplacing(g_data, kStateFile, Utf8(json))) g_lastBody = std::move(body);
}

// ---- commands (folder jobs) ---------------------------------------------------

struct Command {
    std::wstring id, cmd, folder, name, kind, path, dark, light, app, item;
    int index = 0;
    std::vector<std::wstring> apps, names;
    bool badArgs = false;  // a field of the wrong type or size, or no JSON object at all
};

// A pinned folder by name: its path as its pin has it, or empty.
std::wstring PinnedFolder(const std::wstring& name) {
    if (!IsFolderName(name)) return {};
    std::wstring wanted = folders::Root() + L"\\" + name;
    for (const folders::Pin& pin : folders::ReadPins())
        if (std::wstring folder = folders::FolderOfPin(pin);
            !folder.empty() && _wcsicmp(folder.c_str(), wanted.c_str()) == 0)
            return PathIsDirectoryW(folder.c_str()) ? folder : L"";
    return {};
}

// An app to put into a folder: a file by its full path, and no folder's own pin.
PCWSTR CheckApp(const std::wstring& path) {
    if (path.empty() || PathIsRelativeW(path.c_str())) return kBadArgs;
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) return kNotFound;
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) || !folders::FolderFromLnk(path).empty())
        return kBadArgs;
    return nullptr;
}

std::wstring SourceAppId(const std::wstring& path) {
    std::wstring dir = path.substr(0, PathFindFileNameW(path.c_str()) - path.c_str());
    if (!dir.empty()) dir.pop_back();
    return folders::ItemAppId(dir, PathFindFileNameW(path.c_str()));
}

// The pin of the app a shortcut starts, when that app is pinned on its own: the same
// shortcut file, or the same app ID.
int AppPinOf(const std::vector<folders::Pin>& pins, const std::wstring& path) {
    std::wstring key = folders::AppKey(SourceAppId(path));
    for (size_t i = 0; i < pins.size(); i++) {
        if (pins[i].appId.starts_with(folders::kAppIdPrefix)) continue;
        if (_wcsicmp(folders::PathFromPidl(pins[i].pidl.get()).c_str(), path.c_str()) == 0 ||
            (!key.empty() && !pins[i].appId.empty() && folders::AppKey(pins[i].appId) == key))
            return (int)i;
    }
    return -1;
}

// The app goes in last, as a merge puts it: a pinned app by its pin's own item and app
// ID, so its windows join the folder. Returns its file name there.
std::wstring AddApp(const std::wstring& folder, const std::wstring& path,
                    const std::vector<folders::Pin>& pins, int pin) {
    if (pin >= 0) return folders::AddShortcutInto(pins[pin].pidl.get(), folder, pins[pin].appId, true);
    folders::Pidl item = folders::PidlFromPath(path);
    return item ? folders::AddShortcutInto(item.get(), folder, L"", true) : L"";
}

void UnpinApps(const std::vector<std::wstring>& appIds) {
    ComPtr<folders::IPinManagerInterop3> manager = folders::PinManager();
    if (!manager || appIds.empty()) return;
    std::vector<folders::Pin> pins = folders::ReadPins();
    for (const std::wstring& appId : appIds)
        if (int index = folders::FindPin(pins, appId); index >= 0)
            folders::Unpin(manager.Get(), pins[index].pidl.get());
}

// A new folder with the apps, pinned last on the taskbar; pinned apps that went in
// leave the taskbar.
PCWSTR Create(const Command& c) {
    if (!IsFolderName(c.name)) return kBadName;
    if (c.apps.empty()) return kBadArgs;
    for (const std::wstring& app : c.apps)
        if (PCWSTR error = CheckApp(app)) return error;
    std::wstring folder = folders::Root() + L"\\" + c.name;
    if (GetFileAttributesW(folder.c_str()) != INVALID_FILE_ATTRIBUTES) return kExists;
    ComPtr<folders::IPinManagerInterop3> manager = folders::PinManager();
    if (!manager) return kFailed;
    if (!CreateDirectoryW(folder.c_str(), nullptr))
        return GetLastError() == ERROR_ALREADY_EXISTS ? kExists : kFailed;
    std::vector<folders::Pin> pins = folders::ReadPins();
    std::vector<std::wstring> unpin;
    bool added = false;
    for (const std::wstring& app : c.apps) {
        int pin = AppPinOf(pins, app);
        if (AddApp(folder, app, pins, pin).empty()) continue;
        added = true;
        if (pin >= 0) unpin.push_back(pins[pin].appId);
    }
    if (!added || !folders::WritePinLnk(folder, {folders::MakePreviewIcon(folder)}) ||
        !folders::PinAt(manager.Get(), folders::PidlFromPath(folders::PinLnkPath(folder)).get(),
                        (int)pins.size())) {
        folders::RemoveFolder(folder);  // nothing on the taskbar changed yet
        return kFailed;
    }
    UnpinApps(unpin);
    folders::Regroup(folders::FolderKeys(folder));
    return nullptr;
}

PCWSTR Rename(const std::wstring& folder, const Command& c) {
    if (!IsFolderName(c.name)) return kBadName;
    if (c.name == folders::FolderName(folder)) return nullptr;
    std::wstring renamed = folders::Root() + L"\\" + c.name;
    if (_wcsicmp(renamed.c_str(), folder.c_str()) != 0 &&
        GetFileAttributesW(renamed.c_str()) != INVALID_FILE_ATTRIBUTES)
        return kExists;
    return folders::RenameFolderTo(folder, c.name).empty() ? kFailed : nullptr;
}

bool ReadIco(const std::wstring& path, std::string* bytes) {
    return ReadSmallFile(path, kMaxIconBytes, bytes) == Got::kOk && bytes->size() >= 6 &&
           bytes->compare(0, 4, std::string("\0\0\1\0", 4)) == 0;
}

// An .ico Bloom made goes to .pins\.icons as `<folder>.<key><end>`, the key a hash of
// its contents, so a new picture is a new file the taskbar loads afresh.
std::wstring KeepIcon(const std::wstring& folder, const std::wstring& key, PCWSTR end,
                      const std::string& bytes) {
    std::wstring dir = folders::PinsDir() + L"\\" + kIconsDir;
    std::wstring copy = dir + L"\\" + folders::FolderName(folder) + L"." + key + end;
    CreateDirectoryW(dir.c_str(), nullptr);
    if (GetFileAttributesW(copy.c_str()) == INVALID_FILE_ATTRIBUTES &&
        !folders::WriteFileBytes(copy, {bytes.begin(), bytes.end()}))
        return {};
    return copy;
}

std::wstring CopyIcon(const std::wstring& folder, const std::wstring& path) {
    std::string bytes;
    return ReadIco(path, &bytes) ? KeepIcon(folder, HashBytes(bytes), L".ico", bytes) : L"";
}

// Two .ico files, one for a dark taskbar and one for a light one, kept as a pair whose
// file follows the taskbar's theme (folders::RethemeIcons).
PCWSTR ThemedIcon(const std::wstring& folder, const Command& c) {
    for (const std::wstring* path : {&c.dark, &c.light}) {
        if (path->empty() || PathIsRelativeW(path->c_str())) return kBadArgs;
        if (GetFileAttributesW(path->c_str()) == INVALID_FILE_ATTRIBUTES) return kNotFound;
    }
    std::string dark, light;
    if (!ReadIco(c.dark, &dark) || !ReadIco(c.light, &light)) return kBadArgs;
    std::wstring key = HashBytes(dark + light);
    std::wstring darkCopy = KeepIcon(folder, key, L".dark.ico", dark);
    std::wstring lightCopy = KeepIcon(folder, key, L".light.ico", light);
    if (darkCopy.empty() || lightCopy.empty() ||
        !folders::SetFolderIcon(folder, {folders::LightTaskbar() ? lightCopy : darkCopy}))
        return kFailed;
    folders::DeletePreviewIcons(folder, L"");
    return nullptr;
}

PCWSTR Icon(const std::wstring& folder, const Command& c) {
    if (c.kind == L"themed") return ThemedIcon(folder, c);
    if (c.kind == L"auto") {
        folders::IconLocation preview{folders::MakePreviewIcon(folder)};
        if (preview.path.empty()) return kFailed;
        folders::IconLocation now = folders::PinIcon(folder);
        if ((_wcsicmp(now.path.c_str(), preview.path.c_str()) != 0 || now.index != 0) &&
            !folders::SetFolderIcon(folder, preview))
            return kFailed;
        folders::DeletePreviewIcons(folder, preview.path);
        return nullptr;
    }
    if (c.kind != L"file" || c.path.empty() || PathIsRelativeW(c.path.c_str())) return kBadArgs;
    DWORD attributes = GetFileAttributesW(c.path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
        return kNotFound;
    folders::IconLocation icon{c.path, c.index};
    if (_wcsicmp(PathFindExtensionW(c.path.c_str()), L".ico") == 0) {
        icon = {CopyIcon(folder, c.path), 0};
        if (icon.path.empty()) return kFailed;
    }
    if (!folders::SetFolderIcon(folder, icon)) return kFailed;
    folders::DeletePreviewIcons(folder, L"");
    return nullptr;
}

PCWSTR Add(const std::wstring& folder, const Command& c) {
    if (PCWSTR error = CheckApp(c.app)) return error;
    std::vector<folders::Pin> pins = folders::ReadPins();
    int pin = AppPinOf(pins, c.app);
    if (folders::HoldsSame(folder, c.app, pin >= 0 ? pins[pin].appId : SourceAppId(c.app)))
        return kExists;
    std::wstring name = AddApp(folder, c.app, pins, pin);
    if (name.empty()) return kFailed;
    std::vector<std::wstring> keys = folders::ItemKeys(folder, name);
    if (pin >= 0) UnpinApps({pins[pin].appId});
    folders::RefreshPreview(folder);
    folders::Regroup(keys);
    return nullptr;
}

PCWSTR TakeOut(const std::wstring& folder, const Command& c) {
    if (!IsPlainName(c.item, MAX_PATH)) return kBadArgs;
    std::wstring path = folder + L"\\" + c.item;
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return kNotFound;
    if (!folders::IsShortcut(path)) return kBadArgs;  // only shortcuts go back to the taskbar
    folders::TakeOut(folder, path);
    return GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES ? nullptr : kFailed;
}

// The names first, in that order; items they leave out follow in their order.
PCWSTR Order(const std::wstring& folder, const Command& c) {
    if (c.names.empty()) return kBadArgs;
    std::vector<std::wstring> items = folders::FileNames(folders::LoadItems(folder, 0)), order;
    auto find = [](const std::vector<std::wstring>& names, const std::wstring& name) {
        return std::find_if(names.begin(), names.end(), [&](const std::wstring& other) {
            return _wcsicmp(other.c_str(), name.c_str()) == 0;
        });
    };
    for (const std::wstring& name : c.names) {
        auto it = find(items, name);
        if (it == items.end()) return kNotFound;
        if (find(order, name) != order.end()) return kBadArgs;
        order.push_back(*it);
    }
    for (const std::wstring& name : items)
        if (find(order, name) == order.end()) order.push_back(name);
    folders::WriteOrder(folder, order);
    folders::RefreshPreview(folder);
    return nullptr;
}

PCWSTR Dissolve(const std::wstring& folder) {
    folders::DissolveFolder(folder);
    return folders::FindFolderPin(folders::ReadPins(), folder) < 0 ? nullptr : kFailed;
}

PCWSTR Delete(const std::wstring& folder) {
    folders::DeleteFolder(folder);
    return folders::FindFolderPin(folders::ReadPins(), folder) < 0 ? nullptr : kFailed;
}

// Icon files Bloom handed over stay while a folder's pin shortcut points at them, the
// other file of a pair with them.
void PruneIcons() {
    std::wstring root = folders::Root(), dir = folders::PinsDir() + L"\\" + kIconsDir;
    std::vector<std::wstring> keep;
    for (const folders::Entry& entry : folders::ListEntries(root)) {
        if (!entry.isDir) continue;
        std::wstring icon = folders::PinIcon(root + L"\\" + entry.name).path;
        keep.insert(keep.end(), {icon, folders::ThemedTwin(icon, true), folders::ThemedTwin(icon, false)});
    }
    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileW((dir + L"\\*.ico").c_str(), &fd);
    if (find == INVALID_HANDLE_VALUE) return;
    do {
        std::wstring path = dir + L"\\" + fd.cFileName;
        if (std::none_of(keep.begin(), keep.end(), [&](const std::wstring& kept) {
                return _wcsicmp(kept.c_str(), path.c_str()) == 0;
            }))
            DeleteFileW(path.c_str());
    } while (FindNextFileW(find, &fd));
    FindClose(find);
}

PCWSTR Execute(const Command& c) {
    if (!hooks::g_features.folders || folders::Root().empty()) return kFoldersOff;
    if (c.badArgs) return kBadArgs;
    if (folders::g_unloading) return kFailed;
    if (c.cmd == L"create") return Create(c);
    static constexpr PCWSTR kOnFolder[] = {L"rename", L"icon",     L"add",   L"takeOut",
                                           L"order",  L"dissolve", L"delete"};
    if (std::none_of(std::begin(kOnFolder), std::end(kOnFolder),
                     [&](PCWSTR cmd) { return c.cmd == cmd; }) ||
        c.folder.empty())
        return kBadArgs;
    std::wstring folder = PinnedFolder(c.folder);
    if (folder.empty()) return kNoFolder;
    if (c.cmd == L"rename") return Rename(folder, c);
    if (c.cmd == L"icon") return Icon(folder, c);
    if (c.cmd == L"add") return Add(folder, c);
    if (c.cmd == L"takeOut") return TakeOut(folder, c);
    if (c.cmd == L"order") return Order(folder, c);
    if (c.cmd == L"delete") return Delete(folder);
    return Dissolve(folder);
}

// The command file goes first, then the state the command left, then the result:
// Bloom sees the result with the new state already in place.
void Handle(const Command& c, const std::wstring& file) {
    PCWSTR error = Execute(c);
    Wh_Log(L"Bloom command %s %s: %s", c.id.c_str(), c.cmd.c_str(), error ? error : L"ok");
    DeleteFileW((g_inbox + L"\\" + file).c_str());
    if (!error) PruneIcons();
    WriteState();
    SYSTEMTIME now;
    GetSystemTime(&now);
    std::wstring json = L"{\"id\":" + Quote(c.id) + L",\"ok\":" +
                        (error ? L"false,\"error\":" + Quote(error) : std::wstring(L"true")) +
                        L",\"at\":" + Quote(IsoTime(now)) + L"}";
    if (MakeFolders()) WriteReplacing(g_outbox, c.id + L".json", Utf8(json));
}

// ---- watcher thread -------------------------------------------------------------

// Reads a command's fields; false while the text isn't a JSON object yet.
bool ParseCommand(const std::wstring& text, Command* c) {
    wdj::JsonObject root{nullptr};
    if (!wdj::JsonObject::TryParse(text, root)) return false;
    auto cmd = usage::Field(root, L"cmd", wdj::JsonValueType::String);
    auto args = root.TryLookup(L"args");
    if (!cmd || (args && args.ValueType() != wdj::JsonValueType::Object)) {
        c->badArgs = true;
        return true;
    }
    c->cmd = cmd.GetString();
    if (!args) return true;
    wdj::JsonObject fields = args.GetObject();
    // A missing or null field stays empty; any other type, or too long, is bad.
    auto value = [&](PCWSTR key) -> wdj::IJsonValue {
        auto v = fields.TryLookup(key);
        return v && v.ValueType() != wdj::JsonValueType::Null ? v : nullptr;
    };
    auto readText = [&](const wdj::IJsonValue& v, std::wstring* out) {
        if (v.ValueType() != wdj::JsonValueType::String || v.GetString().size() > kMaxText)
            c->badArgs = true;
        else
            *out = v.GetString();
    };
    const std::pair<PCWSTR, std::wstring*> texts[] = {
        {L"folder", &c->folder}, {L"name", &c->name}, {L"kind", &c->kind},
        {L"path", &c->path},     {L"dark", &c->dark}, {L"light", &c->light},
        {L"app", &c->app},       {L"item", &c->item}};
    for (auto [key, out] : texts)
        if (auto v = value(key)) readText(v, out);
    const std::tuple<PCWSTR, std::vector<std::wstring>*, size_t> lists[] = {
        {L"apps", &c->apps, kMaxApps}, {L"names", &c->names, kMaxNames}};
    for (auto [key, out, max] : lists) {
        auto v = value(key);
        if (!v) continue;
        if (v.ValueType() != wdj::JsonValueType::Array || v.GetArray().Size() > max) {
            c->badArgs = true;
            continue;
        }
        for (auto const& item : v.GetArray()) readText(item, &out->emplace_back());
    }
    if (auto v = value(L"index")) {
        double index = v.ValueType() == wdj::JsonValueType::Number ? v.GetNumber() : 0.5;
        if (index != std::floor(index) || index < std::numeric_limits<int>::min() ||
            index > std::numeric_limits<int>::max())
            c->badArgs = true;
        else
            c->index = (int)index;
    }
    return true;
}

HANDLE g_thread;
HANDLE g_stop;
SRWLOCK g_nudgeLock = SRWLOCK_INIT;
HANDLE g_nudge;  // under g_nudgeLock; null while the link is off

// Any thread: something the state shows may have changed.
void Nudge() {
    AcquireSRWLockShared(&g_nudgeLock);
    if (g_nudge) SetEvent(g_nudge);
    ReleaseSRWLockShared(&g_nudgeLock);
}

// Runs `work` on a folder job and waits for it, so jobs run one at a time; none starts
// once unload began. The event is set when the job lets go of its callable, which
// happens too when no thread started.
void RunJob(std::function<void()> work) {
    if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) return;
    auto done = std::make_shared<winrt::handle>(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!*done) return;
    struct Signal {
        explicit Signal(std::shared_ptr<winrt::handle> e) : event(std::move(e)) {}
        ~Signal() { SetEvent(event->get()); }
        std::shared_ptr<winrt::handle> event;
    };
    folders::StartJob([work = std::move(work), signal = std::make_shared<Signal>(done)] {
        core::Guard(L"Bloom link job", work);
    });
    HANDLE waits[] = {done->get(), g_stop};
    if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0 &&
        WaitForSingleObject(done->get(), kJobDrainMs) != WAIT_OBJECT_0)
        Wh_Log(L"Bloom link job still running at unload");
}

// Inbox state between passes: tries of files that didn't read yet, and files handled
// that could not be deleted.
struct Inbox {
    std::unordered_map<std::wstring, int> tries;
    std::unordered_set<std::wstring> handled;
    bool retry = false;
};

std::vector<std::wstring> CommandFiles() {
    std::vector<std::wstring> names;
    WIN32_FIND_DATAW fd;
    HANDLE find = FindFirstFileExW((g_inbox + L"\\*").c_str(), FindExInfoBasic, &fd,
                                   FindExSearchNameMatch, nullptr, 0);
    if (find == INVALID_HANDLE_VALUE) return names;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && IsCommandFile(fd.cFileName))
            names.push_back(fd.cFileName);
    } while (FindNextFileW(find, &fd));
    FindClose(find);
    std::sort(names.begin(), names.end());
    return names;
}

// Every command file in name order. One that is still being written holds back the
// ones after it until it reads, or until it has had its tries.
void ProcessInbox(Inbox& inbox) {
    inbox.retry = false;
    std::vector<std::wstring> files = CommandFiles();
    auto listed = [&](const std::wstring& name) {
        return std::find(files.begin(), files.end(), name) != files.end();
    };
    std::erase_if(inbox.handled, [&](const std::wstring& name) { return !listed(name); });
    std::erase_if(inbox.tries, [&](const auto& entry) { return !listed(entry.first); });
    for (const std::wstring& file : files) {
        if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT) return;
        if (inbox.handled.contains(file)) continue;
        Command c;
        c.id = file.substr(0, file.size() - 5);  // without ".json"
        std::string bytes;
        Got got = ReadSmallFile(g_inbox + L"\\" + file, kMaxCommandBytes, &bytes);
        if (got == Got::kGone) continue;
        bool ready = got == Got::kTooBig || (got == Got::kOk && ParseCommand(usage::Widen(bytes), &c));
        if (!ready && ++inbox.tries[file] < kMaxTries) {
            inbox.retry = true;
            return;
        }
        // Too big, or locked or no JSON object for its every try.
        if (got == Got::kTooBig || !ready) c.badArgs = true;
        inbox.tries.erase(file);
        RunJob([c, file] { Handle(c, file); });
        if (GetFileAttributesW((g_inbox + L"\\" + file).c_str()) != INVALID_FILE_ATTRIBUTES)
            inbox.handled.insert(file);
    }
}

// The taskbar's process only: another explorer (folder windows in a process of their
// own) leaves Bloom's files alone. A starting explorer gets a while to make its taskbar.
bool IsTaskbarProcess() {
    for (int i = 0; i < kTaskbarWaits; i++) {
        if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            DWORD process = 0;
            GetWindowThreadProcessId(tray, &process);
            return process == GetCurrentProcessId();
        }
        if (WaitForSingleObject(g_stop, kTaskbarWaitMs) != WAIT_TIMEOUT) return false;
    }
    return false;
}

// The inbox's directory read. However Watch ends, a read under way is called off and
// waited out before the buffer it writes into goes.
struct InboxRead {
    winrt::file_handle dir;
    OVERLAPPED overlapped{};
    alignas(DWORD) BYTE buffer[1024];
    bool pending = false;
    ~InboxRead() {
        if (!pending) return;
        CancelIoEx(dir.get(), &overlapped);
        DWORD bytes;
        GetOverlappedResult(dir.get(), &overlapped, &bytes, TRUE);
    }
};

constexpr DWORD kRearmMs = 5000;  // an inbox that could not be watched is tried again

void Watch() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    if (!IsTaskbarProcess() || !MakeFolders()) return;
    winrt::handle changed{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
    winrt::handle pinsChanged{CreateEventW(nullptr, FALSE, FALSE, nullptr)};
    if (!changed || !pinsChanged) return;
    // Pin, unpin and reorder on the taskbar itself rewrite this key.
    HKEY taskband = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kTaskbandKey, 0, KEY_NOTIFY, &taskband) != ERROR_SUCCESS)
        taskband = nullptr;
    auto armPins = [&] {
        return taskband && RegNotifyChangeKeyValue(taskband, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                                   pinsChanged.get(), TRUE) == ERROR_SUCCESS;
    };
    InboxRead read;  // after `changed`, which its wait uses
    winrt::file_handle& dir = read.dir;
    OVERLAPPED& overlapped = read.overlapped;
    overlapped.hEvent = changed.get();
    bool& pending = read.pending;
    int failures = 0;  // in a row; an inbox that can't be watched is left alone
    ULONGLONG armDue = 0;
    auto armInbox = [&] {
        ResetEvent(changed.get());
        if (!dir && failures < 3 && MakeFolders())
            dir = winrt::file_handle{CreateFileW(
                g_inbox.c_str(), FILE_LIST_DIRECTORY,
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr)};
        pending = dir && ReadDirectoryChangesW(dir.get(), read.buffer, sizeof(read.buffer), FALSE,
                                               FILE_NOTIFY_CHANGE_FILE_NAME |
                                                   FILE_NOTIFY_CHANGE_LAST_WRITE,
                                               nullptr, &overlapped, nullptr);
        if (!pending) {
            failures++;
            dir.close();
        }
        // Nothing else wakes the loop for an inbox that isn't watched.
        armDue = !pending && failures < 3 ? GetTickCount64() + kRearmMs : 0;
    };
    armPins();
    armInbox();  // before the first pass, so a file that comes meanwhile isn't missed

    Inbox inbox;
    bool scan = true;
    ULONGLONG stateDue = GetTickCount64(), retryDue = 0;
    HANDLE waits[] = {g_stop, changed.get(), g_nudge, pinsChanged.get()};
    for (;;) {
        if (scan) {
            scan = false;
            ProcessInbox(inbox);
            retryDue = inbox.retry ? GetTickCount64() + kRetryMs : 0;
        }
        ULONGLONG now = GetTickCount64();
        if (stateDue && now >= stateDue) {
            stateDue = 0;
            RunJob(WriteState);
        }
        constexpr ULONGLONG kNever = std::numeric_limits<ULONGLONG>::max();
        ULONGLONG due = std::min({stateDue ? stateDue : kNever, retryDue ? retryDue : kNever,
                                  armDue ? armDue : kNever});
        now = GetTickCount64();
        DWORD timeout = due == kNever ? INFINITE : due > now ? DWORD(due - now) : 0;
        DWORD woke = WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, timeout);
        if (woke == WAIT_OBJECT_0 || woke == WAIT_FAILED) break;
        if (woke == WAIT_OBJECT_0 + 1) {
            DWORD bytes = 0;
            pending = false;
            if (GetOverlappedResult(dir.get(), &overlapped, &bytes, FALSE))
                failures = 0;
            else
                dir.close();  // the inbox went away: made again if Bloom is still there
            armInbox();
            scan = true;
        } else if (woke == WAIT_OBJECT_0 + 2 || woke == WAIT_OBJECT_0 + 3) {
            if (woke == WAIT_OBJECT_0 + 3) armPins();
            if (!stateDue) stateDue = GetTickCount64() + kSettleMs;
        } else if (woke == WAIT_TIMEOUT && retryDue && GetTickCount64() >= retryDue) {
            scan = true;
        }
        if (armDue && GetTickCount64() >= armDue) {
            armInbox();
            scan = scan || pending;  // files that came while it wasn't watched
        }
    }
    if (taskband) RegCloseKey(taskband);
}

DWORD WINAPI WatchProc(void*) {
    core::Guard(L"Bloom link", Watch);
    return 0;
}

// ---- lifecycle ------------------------------------------------------------------

// Windhawk thread, after init. Bloom's folder is looked for once, here.
void Start() {
    if (g_thread || !InitPaths()) return;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE nudge = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    AcquireSRWLockExclusive(&g_nudgeLock);
    g_nudge = nudge;
    ReleaseSRWLockExclusive(&g_nudgeLock);
    g_thread = g_stop && nudge ? CreateThread(nullptr, 0, WatchProc, nullptr, 0, nullptr) : nullptr;
    if (!g_thread) {
        Wh_Log(L"Bloom link did not start");
        AcquireSRWLockExclusive(&g_nudgeLock);
        g_nudge = nullptr;
        ReleaseSRWLockExclusive(&g_nudgeLock);
        if (nudge) CloseHandle(nudge);
        if (g_stop) CloseHandle(g_stop);
        g_stop = nullptr;
    }
}

// Windhawk thread, at unload, before the folders stop: a command under way finishes
// first, and nothing of the link runs after this.
void Stop() {
    if (!g_thread) return;
    SetEvent(g_stop);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    AcquireSRWLockExclusive(&g_nudgeLock);
    HANDLE nudge = std::exchange(g_nudge, nullptr);
    ReleaseSRWLockExclusive(&g_nudgeLock);
    CloseHandle(nudge);
    CloseHandle(g_stop);
    g_thread = g_stop = nullptr;
}

}  // namespace bloom_link

// ============================================================================
namespace entry {
// ============================================================================

// Taskbar thread: puts every enabled feature onto the taskbars that exist.
void ApplyAll() {
    core::NoteUiThread();
    core::g_vertical = core::LooksVertical(core::PrimaryTaskbar());
    if (core::g_vertical) {
        Wh_Log(L"Vertical taskbar: shape features stay off");
    }
    for (HWND hwnd : core::Taskbars()) {
        core::Attach(hwnd);
        if (hooks::g_features.pill) {
            pill::Apply(hwnd);
        }
        if (hooks::g_features.tray) {
            tray::Apply(hwnd);
        }
        if (hooks::g_features.autohide) {
            autohide::Apply(hwnd);
        }
        if (hooks::g_features.media) {
            media::Apply(hwnd);
        }
        if (hooks::g_features.usage) {
            usage::Apply(hwnd);
        }
        if (hooks::g_features.island) {
            island::Apply(hwnd);
        }
        if (hooks::g_features.start) {
            start::Apply(hwnd);
        }
        if (hooks::g_features.folders) {
            folders::HideRunningLines(hwnd);
        }
        if (hooks::g_features.panels) {
            panels::Apply(hwnd);
        }
    }
}

// Taskbar thread, after a taskbar's frame loaded: the taskbars that have no
// widgets or island yet get theirs. Each Apply skips a taskbar it has.
void ApplyNew() {
    if (core::g_vertical || core::g_unloading) {
        return;
    }
    for (HWND hwnd : core::Taskbars()) {
        core::Attach(hwnd);
        if (hooks::g_features.autohide) {
            autohide::Apply(hwnd);
        }
        if (hooks::g_features.media) {
            media::Apply(hwnd);
        }
        if (hooks::g_features.usage) {
            usage::Apply(hwnd);
        }
        if (hooks::g_features.island) {
            island::Apply(hwnd);
        }
        if (hooks::g_features.start) {
            start::Apply(hwnd);
        }
        if (hooks::g_features.panels) {
            panels::Apply(hwnd);
        }
    }
}

void RestoreAll() {
    if (hooks::g_features.island) {
        island::Restore();
    }
    pill::Restore();
    tray::Restore();
    if (hooks::g_features.autohide) {
        autohide::Restore();
    }
    if (hooks::g_features.media) {
        media::Restore();
    }
    if (hooks::g_features.usage) {
        usage::Restore();
    }
    if (hooks::g_features.start) {
        start::Restore();
    }
    panels::Restore();
    widgets::Restore();
    core::RestoreAll();
}

// Once per change: the posted pass sets g_vertical again.
std::atomic<bool> g_orientationPending;

void OnOrientationChanged() {
    if (g_orientationPending.exchange(true)) {
        return;
    }
    Wh_Log(L"Taskbar orientation changed");
    core::Post([] {
        g_orientationPending = false;
        RestoreAll();
        ApplyAll();
    });
}

}  // namespace entry

// In ShellHost only panels runs; g_shellHost is set once in Wh_ModInit.
bool g_shellHost;

BOOL Wh_ModInit() {
    Wh_Log(L">");
    settings::Load();
    g_shellHost = panels::InShellHost();
    if (g_shellHost) {
        return panels::InstallShellHost();
    }
    core::g_onOrientationChanged = entry::OnOrientationChanged;
    return hooks::Install();
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (g_shellHost) {
        return;
    }
    hooks::AfterInit();
    if (hooks::g_features.position) {
        position::Apply();
    }
    core::RunOn(core::PrimaryTaskbar(), entry::ApplyAll);
    if (hooks::g_features.size) {
        size::ApplySettings();
    }
    if (hooks::g_features.media) {
        media::Start();
    }
    if (hooks::g_features.usage) {
        usage::Start();
    }
    if (hooks::g_features.folders) {
        folders::Start();
    }
    bloom_link::Start();
    center::Start();
}

namespace changes {
void Stop();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    if (g_shellHost) {
        panels::g_on = false;
        return;
    }
    center::Stop();
    bloom_link::Stop();
    changes::Stop();
    core::g_unloading = true;
    if (hooks::g_features.folders) {
        folders::Stop();
    }
    if (hooks::g_features.media) {
        media::Stop();
    }
    if (hooks::g_features.usage) {
        usage::Stop();
    }
    if (hooks::g_features.size) {
        size::Unapply();
    }
    if (hooks::g_features.island) {
        island::Stop();
    }
    core::RunOn(core::PrimaryTaskbar(), [] {
        entry::RestoreAll();
        hooks::g_iconLoads.clear();
        hooks::g_frameLoads.clear();
    });
    if (hooks::g_features.position) {
        position::Apply();  // back to Windows' edge
    }
    core::DetachAll();
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_shellHost) {
        return;
    }
    if (hooks::g_features.folders) {
        folders::WaitForWork();
    }
    size::WaitForHooks();
    core::WaitForPosted();
    autohide::UnhookLeftover();
}

namespace changes {

// A slider being dragged in Bloom changes a setting ten times a second. Taking
// every feature off the taskbars and putting it back for each made the islands
// blink and the taskbar come out, so the changes of a run are put on once, this
// long after the last of them. The lock keeps a change from reading the settings
// in while the last run's are being put on.
constexpr LONGLONG kApplyAfterMs = 250;
std::mutex g_lock;
PTP_TIMER g_timer;

void Apply() {
    if (hooks::g_features.position) {
        position::Apply();
    }
    core::RunOn(core::PrimaryTaskbar(), [] {
        entry::RestoreAll();
        entry::ApplyAll();
    });
    if (hooks::g_features.size) {
        size::ApplySettings();
    }
    if (hooks::g_features.folders) {
        folders::SettingsChanged();
    }
}

void CALLBACK ApplyLater(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
    std::lock_guard lock(g_lock);
    if (!core::g_unloading) {
        Apply();
    }
}

// Under g_lock.
void Schedule() {
    if (!g_timer) {
        g_timer = CreateThreadpoolTimer(ApplyLater, nullptr, nullptr);
    }
    if (!g_timer) {
        Apply();
        return;
    }
    ULARGE_INTEGER due{.QuadPart = (ULONGLONG)(-kApplyAfterMs * 10000)};
    FILETIME ft{due.LowPart, due.HighPart};
    SetThreadpoolTimer(g_timer, &ft, 0, 0);
}

// At unload, before the features come off: a waiting run is dropped.
void Stop() {
    if (PTP_TIMER timer = std::exchange(g_timer, nullptr)) {
        SetThreadpoolTimer(timer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(timer, TRUE);
        CloseThreadpoolTimer(timer);
    }
}

}  // namespace changes

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    std::lock_guard lock(changes::g_lock);
    settings::Settings was = settings::g;
    settings::Load();
    if (g_shellHost) {
        panels::LoadShellHostSettings();
        return TRUE;
    }
    center::LoadSettings();
    if (hooks::Wanted() != hooks::g_features) {
        *bReload = TRUE;
        return TRUE;
    }
    if (settings::g == was) {
        return TRUE;
    }
    // Only the islands' own settings: they take them where they are.
    settings::Settings rest = settings::g;
    rest.island = was.island;
    if (rest == was) {
        if (hooks::g_features.island) {
            core::Post(island::Restyle);
        }
        return TRUE;
    }
    changes::Schedule();
    return TRUE;
}
