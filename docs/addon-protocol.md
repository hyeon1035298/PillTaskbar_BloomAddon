# 애드온 약속 (호스트, 런타임, Bloom)

`PillTaskbarHost.exe`, `PillTaskbarRuntime.dll`, Bloom이 함께 지키는 이름과 파일, 순서다. 상수는 `common/addon_protocol.h`(GPL 쪽)와 Bloom의 `AddonProtocol.cs`(MIT 쪽)에 같은 값으로 둔다. 하나를 바꾸면 둘 다 바꾸고 `kProtocol`을 올린다.

### 애드온마다 다른 이름: `addon/addon.conf`

같은 런타임·호스트 소스로 다른 모드의 애드온도 만든다. 애드온마다 다른 이름은 그 애드온의 `addon.conf` 한 곳에서 나온다(`KEY="값"` 줄). `tools/build-addon.sh`가 `ADDON_CONF`(기본 `addon/addon.conf`)를 읽고, `tools/make-identity.py`가 `gen/addon_identity.h`(`addon_protocol.h`가 이름을 조립함)와 `gen/identity.json`(`make-manifest.py`가 읽음)을 만든다. 아래 표와 본문의 `PillTaskbar`, `pill-taskbar`는 PillTaskbar의 값이다.

| 키 | 정하는 것 |
|---|---|
| `ADDON_ID` | 데이터 폴더 `<local>\addons\<id>`, `Local\Bloom.Addon.<id>.Host/Stop`, 창 클래스, `WH_MOD_ID`, addon.json `id` |
| `ADDON_NAME` | `<이름>Host.exe`, `<이름>Runtime.dll`, `<이름>.dll`, explorer 쪽 객체 `Local\<이름>.Config/Loaded/Unload/Unloaded/Runtime.%u` |
| `ADDON_VERSION` | `kVersion`, addon.json `version` |
| `ADDON_MOD_SRC`, `ADDON_MOD_VERSION` | 모드 소스(conf 기준 경로), `WH_MOD_VERSION` |
| `ADDON_WINDHAWK_MODS` | 거절 검사에 쓰는 Windhawk 모드 id(3절 코드 12) |
| `ADDON_SYMBOL_MODULES` | 호스트가 `symbols.txt`를 찾는 모듈. `:core`는 Client.Core 패키지 폴더(올라온 사본 우선), 나머지는 System32 |
| `ADDON_SCHEMA`, `ADDON_VALUES_DEFAULTS` | `settings.schema.json`을 싣는지, 모드 YAML 기본값을 덮는 JSON |
| `ADDON_THIRD_PARTY`, `ADDON_NAME_KO/EN`, `ADDON_ABOUT_KO/EN`, `ADDON_LICENSE`, `ADDON_SOURCE`, `ADDON_MIN_BLOOM`, `ADDON_NEEDS` | 묶음의 고지 파일과 addon.json |

이름이 다르므로 두 애드온의 런타임이 한 explorer에 같이 들어가도 뮤텍스, 이벤트, 공유 메모리, `restart` 거절(같은 파일 이름의 런타임)이 서로 닿지 않는다. 런타임 export 이름 `PillTaskbarHookProc`, `WM_COPYDATA` 값, 데이터 폴더 안 파일 이름은 애드온마다 같다(각자 자기 모듈, 자기 창, 자기 폴더에 묻는다). 모드가 `Wh_ModInit` 말고 다른 콜백을 빼거나 인자 없는 `Wh_ModSettingsChanged()`를 쓰면 `addon/addon_entry.cpp`의 약한 기본 함수가 채운다(Windhawk와 같이: 빠진 것은 아무것도 안 함, 옛 형식은 다시 올리지 않음).

## 1. 폴더와 파일

| 무엇 | 위치 | 쓰는 쪽 | 읽는 쪽 |
|---|---|---|---|
| 배포 폴더(읽기 전용) | `<Bloom 설치 폴더>\addons\pill-taskbar\` | 설치 | Bloom |
| 실행용 복사본 | `<local>\addons\pill-taskbar\<version>\` | Bloom | 호스트, explorer |
| 데이터 폴더 | `<local>\addons\pill-taskbar\` | 아래 표 | 아래 표 |
| 사용자 값 | `<data>\addons\pill-taskbar\settings.json` | Bloom | Bloom |

`<local>`은 Bloom의 `Paths.Local`(보통 `%LOCALAPPDATA%\Bloom`), `<data>`는 `Paths.Data`(보통 `%APPDATA%\Bloom`)다. 개발 복사본(`--data`)은 호스트를 띄우지 않는다.

배포 폴더에 든 것: `addon.json`, `PillTaskbarHost.exe`, `PillTaskbarRuntime.dll`, `PillTaskbar.dll`, `symbols.txt`, `THIRD_PARTY.md`, 릴리스 zip에는 `LICENSE`, `SOURCE.txt`도. Bloom은 `addon.json`의 `files`에 적힌 SHA-256을 확인하며 실행용 복사본을 만들고, 호스트를 그 복사본에서 띄운다. explorer가 DLL을 물고 있어도 Bloom 업데이트가 막히지 않게 하려는 것이다.

데이터 폴더 안:

| 파일 | 쓰는 쪽 | 읽는 쪽 | 내용 |
|---|---|---|---|
| `values.json` | Bloom | 런타임 | 모드 설정 값의 평평한 맵 |
| `host-state.json` | 호스트 | Bloom, 호스트 | 호스트 상태, 충돌 기록, 안전 모드 |
| `runtime-state.json` | 런타임 | Bloom, 호스트 | explorer 안 상태 |
| `runtime-panel-state.json` | ShellHost 안 런타임 | 호스트 | ShellHost 안 상태(4절 "ShellHost"). 모양은 `runtime-state.json`과 같다 |
| `prefs.json` | 모드, Bloom | 모드, Bloom | 모드가 쓰는 설정(아래 `prefs.json`) |
| `offsets\<모듈 소문자>_<GUID+age>.txt` | 호스트 | 런타임 | 기호 오프셋 |
| `symbols\<PDB>\<GUID+age>\<PDB>` | 호스트 | 호스트 | 받은 PDB(오프셋을 만든 뒤 지움) |
| `host.log`, `runtime.log`, `runtime-panel.log` | 각자 | 사람 | 기록. 1 MB를 넘으면 처음부터 다시 씀 |

JSON은 모두 UTF-8이고, 쓰는 쪽이 임시 파일에 쓴 뒤 이름을 바꿔 통째로 바꾼다.

### `values.json`

```json
{ "pill.enabled": 1, "pill.style": "windowGlass", "autohide.showDelay": -1, "$log": false }
```

- 키는 모드의 `그룹.키` 이름 그대로다. 값은 정수, `true`/`false`, 문자열이다. 런타임은 불리언을 0/1로 읽는다.
- 없는 키는 런타임이 모드 YAML의 기본값을 돌려준다(Windhawk는 0). 그래서 Bloom은 사용자가 바꾼 키만 써도 된다. Windhawk에서 가져온 값이 있으면 Bloom은 그것을 쓰고, 가져온 적이 없으면 `{}`를 쓴다.
- `$`로 시작하는 키는 런타임 설정이다. `$log`가 참이면 모드의 `Wh_Log`가 `runtime.log`에 남는다.
- 런타임은 이 파일이 바뀌면(내용이 실제로 다를 때만) 다시 읽고 모드의 `Wh_ModSettingsChanged`를 부른다.

#### Bloom이 자기 화면을 따라 정하는 키

설정 › 애드온 페이지에 따로 나오지 않고, Bloom이 테마가 바뀔 때마다(`Theme.Changed`, 유리 효과 스위치도 테마를 다시 적용한다)와 애드온을 시작할 때 쓴다(`AddonHub.FollowTheme`). 애드온 스키마에 그 키가 없으면 쓰지 않는다(옛 애드온).

| 키 | 값 | 뜻 |
|---|---|---|
| `pill.theme` | `dark`, `light` | Bloom이 보이는 라이트·다크. 모드 기본은 `system`(Windows 작업표시줄 테마) |
| `pill.glass` | `true`, `false` | Bloom의 설정 › 유리 효과(`Settings.Transparency`가 `off`가 아님). 기본 `true`라 켜져 있으면 파일에서 빠진다. 키가 없거나 파일이 없으면 유리 그대로(이전 동작) |

#### 숨은 작업표시줄을 붙잡는 창

다른 프로그램의 최상위 창에 창 속성 `PillTaskbar.HoldReveal`(값은 0이 아니면 무엇이든)이 있으면, 포인터가 그 창 위에 있는 동안 숨은 작업표시줄을 꺼내는 포인터 요청(가장자리 닿기, 마우스 올림, 트레이의 호버 타이머)을 모드가 버린다. 아일랜드도 그대로 있다. 포인터가 그 창을 벗어나거나 창·속성이 없어지면 원래대로다. Bloom의 화면 녹화 막대가 아일랜드·알약 자리에서 자라날 때 쓴다(`RecordBar`). 파일이나 설정을 거치지 않으므로 Bloom이 끝나도 남는 것이 없다.

#### 트레이 자리 창 속성

모드는 explorer에서 작업표시줄 창(`Shell_TrayWnd`, `Shell_SecondaryTrayWnd`)마다 창 속성 `PillTaskbar.TraySpan`에 알약 트레이의 화면 x 범위를 둔다. 값은 포인터 크기 정수 하나로, 아래 32비트가 왼쪽, 위 32비트가 오른쪽(둘 다 부호 있는 화면 픽셀)이다. 트레이가 없는 작업표시줄(보조 모니터)은 알약 전체의 범위다. 레이아웃이 바뀔 때마다 고치고, 기능을 끄거나 모드를 내리면 지운다. 빠른 설정과 알림 센터를 놓을 때(`tray.panelsNearTray`) ShellHost 안의 모드와 explorer 안의 모드가 읽는다. 파일이나 공유 메모리를 쓰지 않으므로 explorer가 끝나면 함께 없어진다.

`pill.glass`가 거짓이면 모드는 알약 조각, 숨김 아일랜드가 알약으로 바뀌는 색, 폴더 창 바탕을 블러·백드롭 없이 테마 틴트 색(`SystemChromeMediumColor`, 폴더 창은 팔레트의 surface)으로 불투명하게 칠한다. 윤곽선(Bloom 모양의 옅은 선, 글래스모피즘의 가장자리 빛)은 그대로다. `Wh_ModSettingsChanged`가 알약과 아일랜드를 다시 그리므로 explorer를 다시 시작하지 않고 양쪽으로 바뀐다. 폴더 창은 다음에 열 때 반영된다.

#### 아일랜드 자리 창 속성과 메시지

다른 애드온이 숨김 아일랜드 옆에 자기 아일랜드를 붙일 수 있게, 모드는 작업표시줄 창(`Shell_TrayWnd`, `Shell_SecondaryTrayWnd`)마다 창 속성 `PillTaskbar.Island`에 그 작업표시줄 아일랜드의 자리를 둔다. 아일랜드가 보이고 작업표시줄이 들어가 있는 동안(`Phase::kIdle`)만 있고, 작업표시줄이 나오기 시작하거나 아일랜드가 숨거나(전체 화면 앱, 세로 작업표시줄, 보일 것 없음) 모드를 내리면 지운다. 값은 포인터 크기 정수 하나다.

| 비트 | 뜻 |
|---|---|
| 0–15 | 아일랜드 창의 왼쪽 x(필렛 포함, 부호 있는 화면 픽셀) |
| 16–31 | 오른쪽 x(같음, 끝 픽셀의 다음) |
| 32–47 | 화면 가장자리 y: 위 가장자리면 아일랜드의 위, 아래 가장자리면 아일랜드의 아래(부호 있는 화면 픽셀) |
| 48–59 | 높이(픽셀) |
| 60 | 1이면 아래 가장자리 |
| 61 | 1이면 흰 아일랜드(`island.white`), 0이면 검정 |

값이 바뀔 때마다(놓기, 지우기, Ctrl 드래그로 옮김, 크기·색 변경) 모드는 그 작업표시줄 창에 등록 메시지 `PillTaskbar.IslandChanged`를 `PostMessage`한다(wParam, lParam 0). 받는 쪽은 그 창을 서브클래스해서 듣는다. 작업표시줄 창이 원래 처리하지 않는 메시지라 다른 영향은 없다.

다른 애드온이 등록 메시지 `PillTaskbar.IslandReveal`을 작업표시줄 창에 보내면(wParam, lParam 0), 그 작업표시줄의 아일랜드가 보이고 들어가 있을 때 아일랜드를 클릭한 것과 같이 작업표시줄을 꺼낸다. 옆 아일랜드를 클릭으로 꺼내기에 쓰도록 둔 것이다. 메모리 정리 애드온이 둘 다 쓴다(아일랜드 옆에 표시).

### `host-state.json`

```json
{ "v": 1, "version": "0.1.0", "pid": 4321, "phase": "running", "reason": "",
  "progress": { "done": 0, "total": 0 },
  "safeMode": false,
  "loaded": { "pid": 1234, "created": 133000000000000000, "at": "2026-01-01T00:00:00Z" },
  "crashes": ["2026-01-01T00:00:20Z"],
  "at": "2026-01-01T00:00:30Z" }
```

- `phase`: `starting`, `symbols`(기호 준비, `progress`는 받은 바이트와 전체 바이트), `waiting`(작업표시줄을 기다리거나 충돌 뒤 쉬는 중), `injecting`, `running`, `stopping`, `stopped`, `failed`.
- `reason`(`failed`나 종료 때): `windhawk`, `noSymbols`, `download`, `blocked`, `crashes`, `unloadTimeout`, `error`.
- `loaded`: 넣기 직전에 먼저 쓰는 explorer 기록(pid, 프로세스 생성 시각 FILETIME, 시각). 내린 뒤에는 `null`.
- `crashes`: 센 explorer 충돌 시각들(셈 규칙은 아래 "지키기"). 10분보다 오래된 것은 지운다.
- `safeMode`: 참이면 호스트는 넣지 않고 코드 10으로 끝난다. `--reset-safe-mode`로 띄우면 `crashes`와 함께 비운다.

### `runtime-state.json`

```json
{ "v": 1, "version": "0.1.0", "pid": 1234, "state": "running", "reason": "", "at": "2026-01-01T00:00:31Z" }
```

- `pid`는 explorer pid다. 호스트의 `loaded.pid`와 같을 때만 지금 explorer의 상태다.
- `state`: `starting`, `running`, `reloading`(설정 변경으로 모드 DLL을 다시 올리는 중), `initFailed`, `off`(모드가 설정 변경에 FALSE를 돌려줘 내려 둠), `refused`, `stopped`.
- `reason`: `initFailed`이면 `symbols`(필수 기호 없음이나 오프셋 파일 없음) 또는 `init`. `refused`이면 `windhawk`(Windhawk판 모드가 같은 explorer에 있음), `restart`(앞서 올린 모드 이미지나 다른 `PillTaskbarRuntime.dll`이 explorer에 남아 있음, explorer를 다시 시작해야 함), `protocol`(호스트의 `Config` 크기나 `kProtocol`이 런타임과 다름). `stopped`이면 비어 있거나 `leaked`(내릴 때 모드 DLL을 안전하게 풀 수 없어 explorer에 남겨 둠, 다음 넣기는 `restart`로 거절됨).
- `refused`와 시작 때의 `initFailed`는 `Unload`가 올 때까지 `Loaded`를 켠 채 그 상태로 남고, 내린 뒤에도 `stopped`로 덮지 않는다. 호스트는 언제 읽어도 이유를 본다.
- 런타임은 `starting`을 쓴 뒤에 `Loaded`를 켠다. 그래서 `Loaded`를 본 호스트가 읽는 파일은 앞 런타임의 `stopped`가 아니다.
- 같은 explorer에 다른 런타임이 아직 있으면(뮤텍스 `Local\PillTaskbar.Runtime.<pid>`를 5초 안에 얻지 못함) 두 번째 런타임은 이 파일도 이벤트도 건드리지 않고 조용히 끝난다. 호스트는 `Loaded`가 켜져 있으면 넣지 않으므로 이 길은 경합에서만 생기고, 호스트는 `Loaded`를 못 받아 1로 끝난다.
- `refused`가 실행 중에 생길 수도 있다. 설정 변경으로 모드를 다시 올리다가 모드를 풀 수 없으면 `refused`/`restart`로 남고 모드 없이 `Unload`를 기다린다.

### 오프셋 파일

`offsets\<모듈 소문자 파일 이름>_<GUID 32자리 대문자><age 16진>.txt`, 한 줄, Windhawk 기호 캐시와 같은 모양이다.

```
1#taskbar.dll#<TimeDateStamp 10진>-<SizeOfImage 10진>#<기호>#<RVA 10진>#<기호>##...
```

RVA가 빈 기호는 "찾아봤지만 이 모듈에 없음"이다. 파일에 아예 없는 기호는 런타임이 없음으로 다루고 기록에 남긴다. 호스트는 `symbols.txt`(모드 소스의 `LR"(…)"` 기호 문자열을 빌드 때 뽑은 목록, 한 줄에 하나)의 모든 이름을 세 모듈(`taskbar.dll`, `Taskbar.View.dll`, `SystemTray.dll`) 모두에서 찾는다. 어느 기호가 필수인지는 런타임의 `HookSymbols`가 모드의 표를 보고 정한다.

## 2. 호스트 명령줄

```
PillTaskbarHost.exe --serve <Bloom PID> --bloom <Bloom.exe 경로> --bloom-local <Paths.Local>
                    [--reset-safe-mode] [--log] [--mod <파일 이름>|none]
PillTaskbarHost.exe --stop
PillTaskbarHost.exe --safe-mode
PillTaskbarHost.exe --resolve-symbols --bloom-local <폴더> [--no-download] [--dump <폴더>]
```

- `--serve`: 평소 실행. Bloom이 `ArgumentList`로 띄운다.
- 한 번만 실행: `Local\Bloom.Addon.pill-taskbar.Host` 뮤텍스. 이미 떠 있으면 두 번째 실행은 자기 명령줄을 `WM_COPYDATA`(`dwData = 0x50544831`)로 첫 호스트의 숨은 창(클래스 `Bloom.Addon.pill-taskbar.Host`)에 넘기고 0으로 끝난다. 다만 `--serve`인데 `host-state.json`의 `version`이 자기와 다르면, 먼저 `--stop`을 넘기고 뮤텍스가 풀리기를 15초까지 기다린 뒤 자기가 첫 호스트가 된다(애드온 업데이트).
- `--stop`: 런타임을 내리고(아래 4절) 끝난다. 이벤트 `Local\Bloom.Addon.pill-taskbar.Stop`을 켜는 것과 같다.
- `--safe-mode`: `safeMode`를 켜고 `--stop`처럼 내린다. 떠 있는 호스트가 없으면 파일만 고친다.
- `--reset-safe-mode`: Bloom의 [다시 켜기]. 시작할 때 `safeMode`와 `crashes`를 비운다.
- `--log`: 런타임의 `Wh_Log`를 처음부터 켠다.
- `--mod none`: 런타임만 넣는다(주입 시험용). `--mod <파일>`: 같은 폴더의 다른 모드 DLL(충돌 시험용).
- `--resolve-symbols`: 기호만 준비하고 끝난다. `--no-download`는 PDB를 받지 않고 캐시만 쓴다. `--dump`는 모듈마다 `names-<모듈>.txt`(`RVA 16진\t이름`)를 쓴다.

## 3. 종료 코드

| 코드 | 뜻 | Bloom 상태 줄 |
|---|---|---|
| 0 | 정상(멈춤 요청, Bloom이 사라지고 유예가 끝남) | 꺼짐 또는 다시 띄움 |
| 1 | 그 밖의 오류 | 5초, 30초 뒤 두 번만 다시 띄움 |
| 10 | explorer가 연달아 멈춰 안전 모드 | 멈춤, [다시 켜기] |
| 11 | 필수 기호가 이 Windows에 없음 | 기호 없음 |
| 12 | Windhawk에서 같은 모드가 켜져 있고 Windhawk가 그것을 올릴 수 있음(아래) | Windhawk에서 켜져 있음 |
| 13 | 코드 무결성(SAC)이 DLL을 막음 | 보안 기능이 막음 |
| 14 | 기호 파일을 받지 못함 | 받지 못함, [다시 시도] |
| 15 | explorer를 다시 시작해야 함(앞서 올린 모드나 런타임 이미지가 explorer에 남아 있음, `runtime-state`의 `refused`/`restart`) | explorer 다시 시작 필요 |

코드 15로 끝날 때 `host-state.json`의 `reason`은 `restart`다.

코드 12의 규칙. 레지스트리만으로는 아무것도 올라가지 않고 Windhawk 엔진이 올린다. 그래서 `ADDON_WINDHAWK_MODS`의 어느 id가 `HKLM\SOFTWARE\Windhawk\Engine\Mods\<id>`에서 꺼져 있지 않을(`Disabled`가 0이거나 없음) 때, 다음 가운데 하나면 충돌이다: `windhawk.exe` 프로세스가 있음, `windhawk` 서비스가 스스로 시작함(시작 유형 자동, 지연 자동 포함), explorer에 그 모드 DLL(`<id>_…`)이 이미 있음. Windhawk가 돌지 않고 서비스가 수동이거나 없으면 넣는다. 사용자가 할 일은 셋 중 하나다: Windhawk에서 그 모드 끄기, Windhawk 서비스를 수동 시작으로 두고 Windhawk 끄기, Windhawk 지우기. 호스트는 레지스트리와 서비스를 읽기만 한다.

Bloom은 종료 코드 10–15 뒤에는 스스로 다시 띄우지 않는다.

## 4. 순서

### 넣기

1. 호스트가 기호를 준비한다. 세 모듈의 디스크 파일에서 RSDS를 읽고, 오프셋 파일이 없으면 PDB를 데이터 폴더 `symbols`, Windhawk의 `C:\ProgramData\Windhawk\Engine\Symbols`(읽기만), msdl 순서로 찾아 dbghelp로 푼다.
2. `Shell_TrayWnd`의 스레드와 pid를 찾는다. 없으면 `TaskbarCreated`를 기다린다.
3. `Local\PillTaskbar.Loaded.<pid>`가 이미 켜져 있으면 런타임이 들어 있다. `runtime-state.json`의 `version`이 같으면 넣지 않고 이어받는다. 다르면 `Unload`를 켜고 `Unloaded`를 10초까지 기다린 뒤 넣는다.
4. `host-state.json`에 `loaded`를 먼저 쓴다.
5. 공유 메모리 `Local\PillTaskbar.Config.<pid>`(`addon::Config`)를 만들고, 이벤트 `Loaded`, `Unload`, `Unloaded`(모두 수동 재설정)를 만든다.
6. 실행용 복사본의 `PillTaskbarRuntime.dll`을 호스트에 올려 `PillTaskbarHookProc` 주소를 얻고, `SetWindowsHookExW(WH_GETMESSAGE, …, 작업표시줄 스레드)`, `PostMessage(Shell_TrayWnd, WM_NULL)`. 런타임이 `Loaded`를 켜면(10초 안, 0.5초마다 `WM_NULL`을 다시 보냄) 훅을 푼다.
7. 런타임: 훅 프로시저가 처음 불리면 공유 메모리를 복사하고, 자기 모듈에 참조를 하나 더 잡고(`GetModuleHandleEx`), 제어 스레드를 만든다. `Config`의 크기나 `kProtocol`이 다르면 `dataDir`까지만 읽어 `refused`/`protocol`을 알린다(`dataDir`까지의 필드는 버전이 바뀌어도 자리를 지킨다). 제어 스레드는 런타임 뮤텍스를 5초까지 기다려 얻고(못 얻으면 아무것도 쓰지 않고 끝남), `starting`을 쓴 뒤 `Loaded`를 켠다. 그다음 거절 사유를 본다: 이 이미지가 앞서 모드를 남겼거나 다른 `PillTaskbarRuntime.dll`이 올라 있으면 `restart`, Windhawk판 모드가 있으면 `windhawk`. 거절이 아니면 데이터 폴더 감시를 먼저 시작하고 `values.json`을 읽은 뒤 모드 DLL을 올린다(`Wh_ModInit`, 훅 일괄 적용, `Wh_ModAfterInit`). 결과는 `runtime-state.json`에 쓴다.
8. 호스트는 `runtime-state.json`이 `running`이 되면 `phase: running`. 이때(이미 들어 있던 런타임을 이어받을 때도) `island.enabled`와 `autohide.enabled`가 둘 다 켜져 있고(`values.json`에 없는 키는 호스트 옆 `settings.schema.json`의 기본값, `values.json`이 없으면 모두 기본값) Windows 작업표시줄 자동 숨김이 꺼져 있으면 `ABM_SETSTATE`로 켠다. 끄지는 않는다. 끄는 것은 Bloom의 "숨김 아일랜드 쓰기" 스위치가 한다(끄면 자동 숨김도 끈다). `initFailed`면 내리고 `symbols`는 11, 그 밖은 1. `refused`면 내리고 `windhawk`는 12, `restart`는 15, 그 밖은 1로 끝난다.

같은 explorer에 같은 경로의 런타임을 다시 넣으면 훅 프로시저가 이미 올라 있는 이미지에서 다시 불린다. 제어 스레드가 끝날 때 시작 표시를 지우므로 그 이미지도 처음부터 다시 시작한다(앞서 모드를 남긴 이미지면 `restart`로 거절).

### 지키기

- 호스트는 explorer 프로세스, Bloom 프로세스, `Stop` 이벤트, 창 메시지(`TaskbarCreated`, `WM_COPYDATA`)를 함께 기다린다.
- 런타임이 들어 있는 동안 5초(`kWindhawkPollMs`)마다 코드 12의 규칙을 다시 본다. Windhawk가 나중에 시작했거나 서비스가 자동으로 바뀌어 충돌이 되면 내리기(아래)를 하고 12로 끝난다. 그 사이 몇 초는 두 사본이 같이 걸려 있을 수 있다.
- explorer가 끝나면 종료 코드(상위 비트 `0xC…`)와 응용 프로그램 로그 이벤트 1000(그 pid)으로 충돌인지 본다. 이벤트의 오류 모듈이 애드온의 DLL(런타임, 모드, 다시 올리기 복사본 `<이름>.r<숫자>.dll`)이면 넣은 뒤 지난 시간과 상관없이 세고, 다른 모듈이거나 모듈을 모르면 넣은 뒤 5분 안의 충돌만 센다. 오류 모듈과 오프셋은 `host.log`에 남긴다. 이벤트가 아직 없으면 다음에 넣기 전에 한 번 더 찾는다. 10분 안에 2번이면 `safeMode`를 켜고 10으로 끝난다. 첫 충돌 뒤에는 새 작업표시줄이 뜨고 5초 뒤에 넣는다. 새 explorer에는 작업표시줄에 XAML 아일랜드 창(`Windows.UI.Composition.DesktopWindowContentBridge`)이 생기고 프로세스가 3초 넘게 지났을 때 넣는다. 아일랜드 창이 15초 안에 생기지 않으면 그때 넣고, 프로세스 나이를 못 읽으면 바로 넣는다. 기다리는 동안에도 250ms마다 다시 보며 다른 이벤트를 받는다.
- Bloom 프로세스가 끝나면 10초 기다린다. 그 사이 새 Bloom이 `--serve`를 넘기면 그 pid로 바꿔 이어 간다. 10초가 지나면 내리고 0으로 끝난다.

### 내리기

1. 호스트가 `Local\PillTaskbar.Unload.<pid>`를 켠다.
2. 런타임 제어 스레드: `Wh_ModBeforeUninit`, 모든 훅 끄기(실패하면 한 번 더), `Wh_ModUninit`, 스레드 기다리기, 훅 지우기(실패하면 한 번 더), 모드 DLL `FreeLibrary`(이미지가 실제로 내려가기를 1초까지 봄), MinHook 정리, `runtime-state: stopped`, 로그 닫기, `Loaded` 끄기, 런타임 뮤텍스 놓기, `Unloaded` 켜기, `FreeLibraryAndExitThread`.
   - 스레드 기다리기: 다른 스레드를 하나씩 잠깐 멈춰 레지스터와 스택(RSP부터 최대 1 MB)을 복사하고 바로 다시 돌린 뒤, RIP나 스택 값 가운데 모드 DLL의 코드 구역이나 MinHook 트램펄린 칸을 가리키면서 바로 앞 바이트가 호출 명령(`E8 rel32`, `FF /2`)인 것이 있는지 본다. 있으면 400 ms마다 다시 본다. 멈춘 동안에는 시스템 호출만 하고 할당이나 잠금은 하지 않는다. 오래된 스택 값이 우연히 맞으면 더 기다릴 뿐이다.
   - 시간: `Unload`를 본 때부터 모드 콜백과 스레드 기다리기를 합쳐 8초(`kRuntimeUnloadMs`), 이미지가 내려가기를 보는 데 1초(`kRuntimeUnmapMs`)까지 쓴다. `values.json`의 `$unloadWaitMs`(시험용)는 이 8초를 줄이기만 한다.
   - 기한 안에 스레드가 빠지지 않거나 훅을 끄거나 지울 수 없으면 모드 DLL `FreeLibrary`도 MinHook 정리도 하지 않고 이미지를 남긴다(충돌보다 누수). `runtime-state`는 `stopped`/`leaked`이고, 그 explorer에는 다시 시작할 때까지 모드를 올리지 않는다. 설정 변경으로 다시 올리다 이렇게 되면 `refused`/`restart`로 남는다.
   - `refused`나 시작 때의 `initFailed`로 기다리던 런타임은 상태를 덮지 않고 끝난다.
3. 호스트는 `Unloaded`를 10초(`kUnloadWaitMs`)까지 기다린다. 런타임 쪽 예산(8초 + 1초)은 이보다 짧다. 다만 모드 자체의 `Wh_ModUninit`이 그보다 오래 걸리면 넘길 수 있다. 안 오면 explorer를 건드리지 않고 `reason: unloadTimeout`을 남긴다.
4. Bloom은 끌 때 `Stop`을 켜고 호스트가 끝나기를 12초까지 기다린 뒤에야 프로세스를 죽인다(호스트가 explorer를 10초 기다리므로).

### ShellHost

빠른 설정(Win+A)은 ShellHost.exe가 자기 창(`ControlCenterWindow`)을 스스로 놓는다. 그래서 모드의 패널 부분(`panels`)이 그 안에서 돈다. 모드는 `Wh_ModInit`에서 실행 파일 이름이 `ShellHost.exe`이면 `SetWindowPos` 훅만 걸고 다른 기능은 하나도 켜지 않는다. 알림 센터·달력(ShellExperienceHost.exe, AppContainer)은 explorer가 창을 놓고 클로킹을 푸므로 explorer 안의 모드가 맡고, 그 프로세스에는 넣지 않는다.

1. explorer의 런타임이 `running`이 되면(그 뒤 5초마다, 아직 없으면) 호스트가 `System32\ShellHost.exe`의 최상위 창 하나를 찾는다. ShellHost는 보통 무결성(AppContainer 아님)이고 서명·확장 지점 완화가 꺼져 있어 explorer와 같은 길로 넣는다.
2. 넣기 3–7과 같다. 이름은 ShellHost pid로 만든 `Config`, `Loaded`, `Unload`, `Unloaded`, 런타임 뮤텍스이고, `Config.flags`에 `kConfigPanel`(2)을 더한다. 훅은 그 창의 스레드에 걸고 `WM_NULL`도 그 창에 보낸다. 런타임은 이 표시를 보고 `runtime-panel-state.json`과 `runtime-panel.log`에 쓴다. Bloom은 이 둘을 읽지 않는다.
3. 이미 `Loaded`가 켜져 있으면 `runtime-panel-state.json`의 버전이 같고 살아 있을 때 이어받고, 아니면 먼저 내린다.
4. 15초 안에 `running`·`off`·`reloading`이 되지 않거나 `initFailed`·`refused`(예: Windhawk판 모드가 ShellHost에 있음)이면 내리고, 이 호스트 실행 동안 다시 넣지 않는다. 훅을 걸지 못해도 같다. 어느 경우든 explorer 쪽과 종료 코드는 그대로이고 `host.log`에만 남는다.
5. ShellHost가 끝나면 다시 뜬 뒤 5초 안에 다시 넣는다. 충돌 종료 코드(`0xC…`)로 끝났으면 이 호스트 실행 동안 다시 넣지 않는다. 안전 모드 셈에는 넣지 않는다.
6. 호스트가 끝날 때(어느 길이든) ShellHost의 런타임도 내리기 1–3으로 내린다. explorer가 끝나도 ShellHost 쪽은 그대로 둔다(창 속성이 없으면 패널은 Windows 자리에 열린다).

`--mod none`(런타임만 넣는 시험)이면 ShellHost에는 넣지 않는다. 설정을 꺼도 런타임과 훅은 남고, 훅은 아무것도 바꾸지 않는다(`Wh_ModSettingsChanged`가 다시 올리지 않고 값만 바꾼다).

### 설정 바꾸기

Bloom이 `values.json`을 바꾸면 런타임이 데이터 폴더를 `ReadDirectoryChangesW`로 보다가 다시 읽고 제어 스레드에서 `Wh_ModSettingsChanged`를 부른다. `bReload`가 참이면 모드 DLL만 내렸다 올린다(다시 넣지 않음, 내리기 2와 같은 기다림과 예산). 호스트를 거치지 않는다.

- 감시는 처음 읽기 전에 시작하므로 모드를 올리는 동안 바뀐 값도 놓치지 않는다.
- 이름이 `values.json`인 변경만 다시 읽는다. 로그와 상태 파일 쓰기는 스레드를 깨우기만 한다.
- 파일이 없거나 폴더가 없으면 모든 키가 기본값이다. 파일은 있는데 읽지 못하면(공유 위반이 재시도 뒤에도 계속, 짧게 읽힘, 16 MB 초과) 이전 값을 그대로 두고 1초마다 10번까지 다시 읽는다.
- 폴더를 볼 수 없으면(폴더 없음, 이벤트를 만들지 못함, `ReadDirectoryChangesW` 실패) 핸들을 닫고 5초마다 다시 열어 보며 그동안 5초마다 파일을 읽는다.

## 5. 작업표시줄 상태와 폴더 명령

모드가 Windhawk판이든 애드온판이든 같은 소스로 돈다. 폴더 위치는 `%LOCALAPPDATA%\Bloom\addons\pill-taskbar\`(이하 `<data>`)로 고정한다. `%LOCALAPPDATA%\Bloom`이 없으면(이 PC에 Bloom이 없음) 모드는 아무것도 만들지 않는다. `--data`로 띄운 Bloom은 그 폴더 안의 `local`을 보므로 실제 explorer와 닿지 않는다.

### `state.json` (모드가 씀, Bloom이 읽음)

고정이나 폴더 내용이 바뀔 때만 다시 쓴다(내용이 같으면 쓰지 않음). 임시 파일 후 이름 바꾸기.

```json
{ "v": 1, "pid": 1234, "writtenAt": "2026-01-01T00:00:00Z",
  "folders": { "enabled": true, "path": "C:\Users\you\TaskbarFolders" },
  "pinned": [
    { "kind": "app", "name": "Chrome", "path": "C:\...\User Pinned\TaskBar\Google Chrome.lnk", "appId": "Chrome" },
    { "kind": "folder", "name": "Work" },
    { "kind": "running" } ],
  "items": {
    "Work": { "icon": { "kind": "auto" },
              "apps": [ { "file": "Code.lnk", "name": "Visual Studio Code", "path": "C:\Users\you\TaskbarFolders\Work\Code.lnk", "appId": "Microsoft.VisualStudioCode" } ] } },
  "running": { "enabled": true, "name": "실행 중",
               "apps": [ { "name": "Explorer", "appId": "Microsoft.Windows.Explorer", "path": "" } ] } }
```

- `pinned`: 작업표시줄 고정 순서. `kind`는 `app`(폴더 밖 고정 앱), `folder`(폴더 하나, `name`은 폴더 이름), `running`(실행 중 자동 폴더 자리, 켜져 있을 때만).
- `items`: 폴더마다 `icon`(`auto` = 앱 모음 미리보기, `file` = `path`와 `index`)과 안의 앱(`.order` 순서). `path`는 Bloom이 아이콘을 뽑을 때 쓰는 바로 가기 파일 경로다.
- `running`: 실행 중 폴더 설정(`folders.autoRunning`, `folders.runningName`)과 지금 든 앱.
- `pid`는 explorer pid, `writtenAt`은 UTC.

### 명령 `inbox\<id>.json` (Bloom이 씀) → 결과 `outbox\<id>.json` (모드가 씀)

`<id>`는 Bloom이 만드는 단조 증가 문자열(예 `20260101T000000123-0001`)이고 모드는 이름 순서로 하나씩 처리한다. 모드는 처리한 inbox 파일을 지우고 결과를 쓴다. Bloom은 읽은 outbox 파일을 지운다. 5초 안에 결과가 없으면 Bloom은 편집 칸을 읽기 전용으로 두고 이유를 한 줄로 보인다.

```json
{ "id": "20260101T000000123-0001", "cmd": "create", "args": { "name": "Work", "apps": ["C:\...\Code.lnk"] } }
{ "id": "20260101T000000123-0001", "ok": false, "error": "exists", "at": "2026-01-01T00:00:00Z" }
```

| `cmd` | `args` | 모드가 하는 일 |
|---|---|---|
| `create` | `name`, `apps[]`(바로 가기 경로) | 하위 폴더, 바로 가기 복사, `.order`, 폴더를 작업표시줄 끝에 고정, 폴더로 들어간 고정 앱의 고정 풀기 |
| `rename` | `folder`, `name` | `RenameFolderTo` |
| `icon` | `folder`, `kind: "auto"` | 미리보기 아이콘 다시 만들기 |
| `icon` | `folder`, `kind: "file"`, `path`, `index` | `.pins\.icons\<폴더>.<hash>.ico`로 복사(`.ico`가 아니면 그대로 경로)하고 `SetFolderIcon` |
| `icon` | `folder`, `kind: "themed"`, `dark`, `light`(`.ico` 경로 둘: 어두운 작업표시줄용, 밝은 작업표시줄용) | `.pins\.icons\<폴더>.<hash>.dark.ico`·`.light.ico`로 복사하고 지금 테마 쪽을 `SetFolderIcon`. 설정이 바뀔 때와 시작할 때 `<이름>.dark.ico`·`<이름>.light.ico` 쌍인 폴더 아이콘은 테마 쪽 파일로 바뀜(`RethemeIcons`) |
| `add` | `folder`, `app`(바로 가기 경로) | 바로 가기 복사, 순서 끝, 그 앱이 고정돼 있었으면 고정 풀기 |
| `takeOut` | `folder`, `item`(파일 이름) | `TakeOut`(폴더 옆에 고정) |
| `order` | `folder`, `names[]`(파일 이름) | `.order` 쓰기 |
| `dissolve` | `folder` | `DissolveFolder`(폴더는 휴지통) |
| `delete` | `folder` | `DeleteFolder`(앱을 다시 고정하지 않고 폴더째 빼고, 폴더는 휴지통) |

구현 규칙:

- `pinned`와 `items`에는 작업표시줄에 고정된 폴더만 나온다. 디스크에만 있는 폴더를 가리키는 명령은 `noFolder`.
- 폴더 기능이 꺼져 있어도 `state.json`은 쓴다(`folders.enabled: false`, 고정 항목은 모두 `app`).
- 모드는 작업표시줄 고정·풀기·순서 변경을 `HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Taskband` 알림으로도 잡아 다시 쓴다. 탐색기에서 폴더 안 파일을 직접 고친 것은 보지 않는다.
- 실행 중 폴더의 창이 닫힌 것은 다음 쓰기 때 빠진다.
- 대소문자만 바꾸는 `rename`은 `failed`.
- `create`는 앱이 하나 이상이어야 하고, 없는 앱이 섞이면 아무것도 만들지 않고 `notFound`. `order`의 모르는 이름은 `notFound`, 중복은 `badArgs`, 빠진 항목은 원래 순서대로 뒤에 붙는다.
- `%LOCALAPPDATA%\Bloom`은 모드가 올라올 때 한 번만 본다.

`error` 값: `foldersOff`(폴더 기능 꺼짐), `noFolder`, `exists`, `badName`, `notFound`(앱이나 항목 없음), `badArgs`, `failed`. 명령이 끝나면 모드는 `state.json`을 다시 쓴다. `.png` 아이콘은 Bloom이 `.ico`로 바꿔 넘긴다.

### `prefs.json`

```json
{ "islandPositions": { "DEL41D9_5_1a2b3c_0_UID4352": { "name": "DEL41D9 2560×1440", "x": 500 } } }
```

- `islandPositions`: 모니터마다 숨김 아일랜드가 가장자리의 어디에 있는지. `x`는 0(왼쪽 끝)~1000(오른쪽 끝), 500이 가운데. 키는 모니터 장치 경로의 하드웨어·인스턴스 부분(영숫자와 `-`, 나머지는 `_`), `name`은 Bloom이 보이는 이름.
- 모드는 아일랜드를 처음 놓는 모니터를 가운데(500)로 더하고, Ctrl을 누른 채 끌어 놓거나 Ctrl+우클릭으로 가운데로 돌릴 때 그 모니터의 항목을 쓴다. 다른 필드와 다른 모니터 항목은 그대로 둔다.
- 모드는 데이터 폴더의 변경 알림으로 이 파일이 바뀐 것을 보고(자기 쓰기 뒤의 마지막 쓴 시각과 다를 때만) 다시 읽는다.

## 6. 설정 스키마 `settings.schema.json`

`tools/make-schema.py --schema`가 모드 YAML, `Clamped` 줄, `tools/names-en.tsv`에서 만든다. 배포 폴더에 들고 `addon.json`의 `"settings"`가 이 파일을 가리킨다.

```json
{ "schema": 1, "groups": [
  { "id": "size", "name": { "ko": "크기", "en": "Size" },
    "items": [
      { "key": "size.enabled", "type": "bool", "default": true, "name": { "ko": "…", "en": "…" }, "about": { "ko": "…", "en": "…" } },
      { "key": "size.iconSize", "type": "int", "default": 32, "min": 8, "max": 128, "name": { … } },
      { "key": "pill.style", "type": "choice", "default": "windowGlass",
        "options": [ { "value": "glass", "name": { "ko": "…", "en": "…" } } ], "name": { … } },
      { "key": "folders.foldersPath", "type": "text", "default": "%USERPROFILE%\TaskbarFolders", "name": { … } } ] } ] }
```

- 그룹에도 YAML에 `$description`이 있으면 `about`(`{"ko","en"}`)이 붙는다(지금 tray, size, start).
- `key`는 `values.json`과 같은 `그룹.키` 전체 이름. `type`은 `bool`, `int`, `choice`, `text`. `about`은 YAML에 `$description`이 있을 때만.
- 글은 `{"ko","en"}`(일본어·중국어는 없으면 Bloom이 영어로 보인다). `en`이 빠지면 스크립트가 실패한다.
- 모드 소스의 `==BloomFileSettings==` 블록에 적은 항목(`"file"`, `"field"`가 있는 것, 지금 `island.positions` records)은 `ADDON_FILE_SETTINGS=1`로 빌드할 때(`make-schema.py --with-files`)만 그 그룹 끝에 붙는다. Bloom이 `values.json`이 아니라 그 파일의 필드를 읽고 쓴다. 그 종류를 모르는 Bloom은 스키마 전체를 버리므로, 읽는 Bloom이 나온 뒤에 켠다.

## 7. 사용량 파일에 더하는 필드 (작업표시줄 사용량 막대)

`%LOCALAPPDATA%\Bloom\export\ai-usage.json`(Bloom이 쓰는 파일)에 필드만 더한다. `v`는 1 그대로이고, 모르는 필드는 무시한다. 빠진 필드는 지금 동작 그대로다.

| 필드 | 값 | 뜻 |
|---|---|---|
| `tag` (맨 위) | `none`, `name`, `plan` | 이름표. `name`은 `label`(별칭)이 있으면 그것, 없으면 `name`. `plan`은 `plan`. 비면 그리지 않는다. 길면 모드가 …로 줄인다. 트레이와 숨김 아일랜드에 같이 나온다. 빠지면 `none` |
| `accounts[].bar` | 그 계정 `limits[].key` 하나(예 `5h`, `week`, `fable`) | 작업표시줄에 보일 한도. 빠졌거나 그 key가 없으면 지금 규칙(주간 막대) |
| `accounts[].icon` | 아이콘 파일 절대 경로(`.svg`, `.png`, `.ico`) | 사용자가 설정 › AI 계정에서 고른 로고. 빠지면 `kind`의 첫 글자. 파일이 없거나 못 읽으면 글자 타일(`kind`의 첫 글자, Claude `C`, ChatGPT `G`) |

아이콘 색(원래 색, 단색)은 파일이 아니라 `values.json`의 `usage.logoColor`다. Bloom은 홈 카드와 같은 "아이콘 색" 설정 값을 그 키에도 쓴다. 모드에는 회사 로고가 들어 있지 않다.
