# Compare Sorting: Merge / Quick / Heap

고급알고리즘 과제 1. 병합·첫 피벗 퀵·힙 정렬을 비교하고, 랜덤 퀵 정렬의 직렬/OpenMP 1·2·4스레드를 추가 비교합니다.

## 실행

GCC, GNU Make, Python 3, OpenMP가 있는 Linux 환경에서:

```sh
make test
make bench
```

Windows에서는 수업 template의 GitHub Codespaces를 권장합니다. 이 프로젝트의 Dockerfile / compose.yml / .devcontainer는 해당 template을 바탕으로 합니다. Docker가 설치되어 있다면 저장소 폴더에서:

```sh
docker compose up -d
docker compose exec lab bash
make test
make bench
```

`make bench`는 기존 측정 CSV를 새 값으로 덮어씁니다. 보고서에 실린 원본을 보존하려면 먼저 results/를 복사해 두세요. 실행마다 진행률, 현재 조건, 시각, 단계·누적 소요시간을 출력합니다. 정상 소요시간은 환경에 따라 달라집니다.

## 그래프 및 보고서 재생성

실험 자체는 Python 표준 모듈과 GCC/OpenMP만 사용합니다. 문서 생성에만 matplotlib/reportlab이 필요합니다.
이미 이 패키지가 있는 Python 환경 또는 별도 가상환경에서:

```sh
python3 -m pip install -r requirements-report.txt
make charts
python3 tools/build_report.py --repo-url https://github.com/실제계정/실제저장소
```

가상환경에서는 `make charts` 대신 `python tools/analyze.py`를 사용해 선택한 Python을 지정할 수도 있습니다.
Debian의 system Python이 pip 설치를 거부하면 system을 강제로 변경하지 말고 venv가 있는 별도 Python 환경을 사용하세요.
`report/submission.json`에 실제 repository_url과 필요하면 author, student_id를 입력할 수 있습니다.
**저장소: https://github.com/snowcover1535/sorting-hw1-2026**

보고서에는 이 저장소 URL이 반영되어 있습니다.

## 주요 파일

| 경로 | 내용 |
|---|---|
| src/sort.c, src/sort.h | 정렬 3종 + 랜덤 직렬/병렬 퀵 |
| src/main.c | 입력 생성·시간 측정·순열 검사 |
| tests/test_sort.c | 1,837개 정확성 검사 |
| tools/run_benchmark.py | 실행·환경 수집·진행 로그 |
| tools/analyze.py | 원본 CSV 집계·그래프 |
| tools/build_report.py | 8쪽 PDF 및 편집 가능한 Markdown 생성 |
| results/ | 원본 CSV, 로그, 요약 통계, 실행환경 |
| report/REPORT.md | 편집 가능한 보고서 |
| report/sorting_comparison_report.pdf | PDF 보고서 |
| SUBMISSION.md | GitHub 업로드 및 제출 절차 |

## 결과 읽기

- 시간 CSV 644행 = 기본 비교 420회 + 병렬 비교 224회. counts.csv는 별도 계측 60회입니다.
- 각 조건은 같은 입력·피벗 seed로 준비 실행 1회 후 7회 측정합니다. 서로 다른 데이터의 평균 성능 실험은 아닙니다.
- summary.csv의 median/q1/q3는 실제 시간값의 중앙값 및 inclusive quartile입니다.
- 그래프의 병렬 speedup은 같은 반복 번호끼리 나눈 비율의 중앙값/IQR입니다. 본문의 배수는 시간 중앙값끼리의 비율입니다.
- counts.csv의 시간은 계측 부하가 섞여 있으므로 성능 시간으로 사용하지 않습니다.
- `moves`는 swap·merge 대입 수입니다. 피벗의 지역 복사는 제외하며 전체 메모리 트래픽이 아닙니다.
- quick_first는 작은 쪽만 재귀 호출하여 스택 O(log n)을 보장하지만 작업량 O(n²)의 최악은 그대로 남습니다.
- buffer_bytes는 보조 배열만 셉니다. 프로세스 RSS·스택·OpenMP task 메모리는 포함하지 않습니다.
- random/parallel CSV의 카운터와 depth=0은 미계측 값입니다.

## 검증

```sh
make test
make sanitize
```

ASan/UBSan 검사를 제공하며 이 환경에서는 통과했습니다. LeakSanitizer는 실행 환경의 /proc 접근 제한 때문에 비활성화했습니다. 누수 검사 통과를 뜻하지 않습니다.

AI 지원으로 설계·구현·보고서 초안을 작성했습니다. 표와 그래프의 수치는 실제 실행 결과입니다. 학습 설명은 보고서 2·8쪽에 포함되어 있습니다.
