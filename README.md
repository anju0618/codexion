*This project has been created as part of the 42 curriculum by amakino*

# ENG VER
# Codexion

## Description
"Codexion" is a concurrency and synchronization simulation project set in a multi-threaded environment where multiple coders compete for limited resources (USB dongles).
Each coder operates independently as a thread, repeating a cycle of "compiling," "debugging," and "refactoring." Compiling quantum code requires two USB dongles simultaneously, but these dongles are shared with adjacent coders. The goal is to achieve optimal schedule management while preventing Deadlock, resource Starvation, and coder Burnout.

## Instructions
### Compilation
Execute the following command in the root directory of the project:
```bash
make
```
This will generate the executable file named codexion.

### Execution
Run the program by specifying the following arguments:
```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```
number_of_coders: The number of coders (and also dongles).
time_to_burnout: If a coder does not start their next compilation within this many milliseconds since their last compilation started, they will burn out.
time_to_compile: The time it takes to compile (in milliseconds).
time_to_debug: The time it takes to debug (in milliseconds).
time_to_refactor: The time it takes to refactor (in milliseconds).
number_of_compiles_required: The simulation ends if everyone compiles this many times.
dongle_cooldown: The cooling time (in milliseconds) required before a released dongle can be used again.
scheduler: The queue scheduling policy. Either fifo or edf (Earliest Deadline First).

#### Example
```bash
./codexion 5 800 200 200 200 5 10 edf
```

## Resources
[POSIX Threads (pthreads) Documentation](https://hpc-tutorials.llnl.gov/posix/)
[Mutexes and Condition Variables](https://computing.llnl.gov/tutorials/pthreads/)
[Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
#### AI Usage
In this project, AI (LLM) assistance was utilized for brainstorming structural design ideas, double-checking Data Races, and formatting this README.

## Blocking cases handled
This project controls and resolves the following critical issues in concurrent processing:

Deadlock Prevention:
To break the "Circular Wait" (one of Coffman's conditions), a Resource Hierarchy is introduced. Coders always acquire the lock for the dongle with the "smaller absolute ID" first, completely preventing deadlocks.

Starvation Prevention:
To prevent starvation caused by resource contention, a custom Priority Heap Queue is implemented for each dongle. Fair resource allocation is achieved by applying scheduling algorithms such as fifo (First In, First Out) and edf (Earliest Deadline First).

Dongle Cooldown Handling:
The cooldown time after releasing a dongle is strictly calculated, and a system is introduced where threads wait for the time to pass using Condition Variables.

Log Serialization:
To prevent two messages from interleaving, an output-dedicated Mutex (print_mutex) is used to protect standard output.

Precise Burnout Detection:
An independent monitor thread continuously checks the state in millisecond increments (precise_usleep(1)), detecting Burnout within a 10ms delay and safely halting all threads.

## Thread synchronization mechanisms

The thread synchronization mechanisms used in this implementation are as follows:

pthread_mutex_t (Mutex):

dongle->mutex: Used to protect each dongle's ownership (holder_id), waiting queue, and cooldown information.

state_mutex: Protects the simulation stop flag (stop_flag), each coder's compile count, deadline, and request time to prevent Data Races.

print_mutex: A locking mechanism to prevent duplication and overlapping in standard output.

pthread_cond_t (Condition Variable):

dongle->cond: When a dongle is in use or on cooldown, attempting threads enter a waiting state using pthread_cond_wait to avoid unnecessary CPU resource consumption. When a dongle is released, pthread_cond_broadcast is called to awaken waiting threads, enabling efficient inter-thread communication. It is also used during burnout detection by the monitor to safely guide all threads to termination.

# JAPANESE VER
*This project has been created as part of the 42 curriculum by amakino*

# Codexion

## Description
「Codexion」は、限られたリソースを複数のコーダーが競い合う、マルチスレッド環境における並行処理と同期のシミュレーションです。
各コーダーはスレッドとして独立して動作し、「コンパイル」「デバッグ」「リファクタリング」のサイクルを繰り返します。量子コードのコンパイルには同時に2つのUSBドングルが必要ですが、ドングルは隣り合うコーダーと共有されています。デッドロックやリソースの枯渇（Starvation）、そしてコーダーの燃え尽き（Burnout）を防ぎながら、最適なスケジュール管理を行うことが目標です。

## Instructions
### コンパイル方法
プロジェクトのルートディレクトリで以下のコマンドを実行してください。
```bash
make
```
これにより、実行可能ファイル codexion が生成されます。

### 実行方法
以下の引数を指定してプログラムを実行します。
```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```
number_of_coders: コーダー（およびドングル）の数
time_to_burnout: 最後のコンパイル開始からこのミリ秒以内に次のコンパイルを始めないとBurnoutする
time_to_compile: コンパイルにかかる時間（ミリ秒）
time_to_debug: デバッグにかかる時間（ミリ秒）
time_to_refactor: リファクタリングにかかる時間（ミリ秒）
number_of_compiles_required: 全員がこの回数コンパイルすればシミュレーション終了
dongle_cooldown: ドングル解放後、次に使えるようになるまでの冷却時間（ミリ秒）
scheduler: キューのスケジューリングポリシー。fifo または edf（Earliest Deadline First）

#### 実行例
```bash
./codexion 5 800 200 200 200 5 10 edf
```

## Resources
[POSIX Threads (pthreads) Documentation](https://hpc-tutorials.llnl.gov/posix/)
[Mutexes and Condition Variables](https://computing.llnl.gov/tutorials/pthreads/)
[Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
#### AI Usage
本プロジェクトでは、コードの構造設計時のアイデア出し、Data Raceのチェック、およびこのREADMEのフォーマット作成においてAI（LLM）の支援を活用しています。

## Blocking cases handled
本プロジェクトでは、並行処理における以下の問題を制御・解決しています

Deadlock（デッドロック）の防止:
Coffmanの条件のうち「環状待機（Circular Wait）」を破壊するため、リソース階層（Resource Hierarchy）を導入しています。コーダーは常に「絶対IDが小さい方のドングル」から順番にロックを取得することで、デッドロックを完全に防いでいます。

Starvation（飢餓）の防止:
リソースの奪い合いによるStarvationを防ぐため、各ドングルに独自の「優先度付きヒープキュー」を実装しました。fifo（先着順）やedf（デッドラインが近い順）のスケジューラを適用することで、公平なリソースの分配を実現しています。

ドングルのクールダウン処理:
ドングル解放後のクールダウン時間を厳密に計算し、時間経過を条件変数を用いて待機する仕組みを導入しています。

ログのシリアライズ:
2つのメッセージが混ざることを防ぐため、出力専用のMutex（print_mutex）を使用して標準出力を保護しています。

正確なBurnout検知:
独立したモニタースレッドが数ミリ秒単位（precise_usleep(1)）で監視を行い、遅延10ms以内でBurnoutを検知して全スレッドを安全に停止させます。

## Thread synchronization mechanisms

本実装で使用しているスレッド同期のメカニズムは以下の通りです

pthread_mutex_t (ミューテックス):

dongle->mutex: 各ドングルの所有権（holder_id）や待機キュー、クールダウン情報を保護するために使用します。

state_mutex: 終了フラグ（stop_flag）や、各コーダーのコンパイル回数、最終期限（deadline）、リクエスト時間などを保護し、Data Raceを防ぎます。

print_mutex: 出力の重複を防ぐためのロック機構です。

pthread_cond_t (条件変数):

dongle->cond: ドングルが使用中、あるいはクールダウン中の場合、取得を試みるスレッドは無駄なCPUリソースを消費しないよう pthread_cond_wait で待機状態に入ります。ドングルが解放されたタイミングで pthread_cond_broadcast を呼び出し、待機中のスレッドを呼び覚ますことで、効率的なスレッド間通信を実現しています。モニターによる終了検知時にも利用され、全スレッドを安全に終了へ導きます。
