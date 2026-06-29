*This project has been created as part of the 42 curriculum by amakino.*

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

[[C言語] pthread/mutexを使った並列処理](https://qiita.com/ryo_manba/items/e48faf2ba84f9e5d31c8)

[相互排他ロックの使用方法](https://docs.oracle.com/cd/E19455-01/806-2732/6jbu8v6op/index.html)

[入門者向け！C言語でのマルチスレッドをわかりやすく解説](https://daeudaeu.com/multithread/)

[マルチスレッド・プログラムの開発](https://www.ibm.com/docs/ja/aix/7.1.0?topic=programming-developing-multithreaded-programs)

[C言語で正確にミリ秒マイクロ秒単位まで計測する方法](https://8ttyan.hatenablog.com/entry/2015/02/03/003428)

[【C言語】時刻を扱う（time・localtime・strftimeなど）](https://daeudaeu.com/c-time/)

[時間情報の取得方法と扱い方](https://www.mm2d.net/main/prog/c/time-04.html)

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

## Mathematical Model for Burnout Prevention (Survival Analysis)

To scientifically evaluate whether a given set of simulation parameters is execution-safe or guaranteed to cause resource starvation, we can model a coder's complete behavioral cycle using the following formula:

$$T_{\text{cycle}} = t_{\text{compile}} + t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}} + W$$

Where $W$ represents the total waiting time for both adjacent USB dongles to become unoccupied. A coder will inevitably experience **burnout** if the total cycle time exceeds their lifetime threshold ($T_{\text{cycle}} > t_{\text{burnout}}$).

### Analyzing the Resource Waiting Time ($W$)
* **Best-Case Scenario ($W_{\text{min}}$):** Both left and right dongles are instantly available the exact millisecond the coder finishes refactoring ($W_{\text{min}} = 0$).
* **Worst-Case Scenario ($W_{\text{max}}$):** Due to resource sharing topography, the upper bound of $W$ changes dynamically depending on whether the total `number_of_coders` is **even** or **odd**.

#### Case 1: Even Number of Coders
When the total number of coders is even, they can seamlessly alternate compiling windows in perfectly synchronized pairs (e.g., in a 4-coder cluster, coders 1 & 3 compile simultaneously, then coders 2 & 4 alternate).
* The worst-case delay $W_{\text{max}}$ is mathematically capped by exactly **1 compile time and 1 cooldown time** of a single neighbor.
* **Maximum Execution Cycle Formula:**
$$T_{\text{cycle-max}} = 2 \times t_{\text{compile}} + 2 \times t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}}$$
* If $t_{\text{burnout}} > T_{\text{cycle-max}}$, the execution environment is fundamentally safe from starvation.

#### Case 2: Odd Number of Coders
When the total number of coders is odd, a symmetrical alternating cycle is mathematically impossible, introducing a staggered, non-linear waiting queue.
* In the worst-case configuration, a coder can get trapped waiting sequentially for **both** adjacent neighbors to finish their respective resource usage. This inflicts a penalty of **2 compile times and 2 cooldown times**.
* **Maximum Execution Cycle Formula:**
$$T_{\text{cycle-max}} = 3 \times t_{\text{compile}} + 3 \times t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}}$$
* Odd-numbered simulations inherently demand a significantly larger $t_{\text{burnout}}$ window to allow all threads to survive.

### 📋 On-the-spot Calculation Worksheet for Evaluation

Reviewers can use this quick worksheet to verify whether a given command line input is mathematically designed to **Survive** or **Burnout**.

#### Step 1: Extract the Variables from the Command
When your peer runs a command like:
`./codexion [num_coders] [time_burnout] [time_compile] [time_debug] [time_refactor] [num_req] [cooldown] [scheduler]`

Plug the numbers into these variables:
* $N$ = `num_coders`
* $T_{\text{burnout}}$ = `time_burnout`
* $t_{\text{comp}}$ = `time_compile`
* $t_{\text{cool}}$ = `dongle_cooldown`
* $t_{\text{dbg}}$ = `time_debug`
* $t_{\text{ref}}$ = `time_refactor`

#### Step 2: Choose the Formula Based on Coder Count ($N$)

* **If $N$ is EVEN (2, 4, 6, 8...):**
$$T_{\text{cycle-max}} = (2 \times t_{\text{comp}}) + (2 \times t_{\text{cool}}) + t_{\text{dbg}} + t_{\text{ref}}$$

* **If $N$ is ODD (3, 5, 7, 9...):**
$$T_{\text{cycle-max}} = (3 \times t_{\text{comp}}) + (3 \times t_{\text{cool}}) + t_{\text{dbg}} + t_{\text{ref}}$$

#### Step 3: Compare and Judge the Result
Compare your calculated $T_{\text{cycle-max}}$ against the input $T_{\text{burnout}}$:

* 🟢 **If $T_{\text{burnout}} > T_{\text{cycle-max}}$ :** 
  The environment provides enough time margin. **Expected Result: All Coders MUST Survive and complete the simulation.** (If someone burns out, there is a thread scheduling bug or a data race!)
* 🔴 **If $T_{\text{burnout}} \le T_{\text{cycle-max}}$ :** 
  The environment is highly restrictive and resource contention will eventually cause starvation. **Expected Result: At least one coder is highly likely to Burnout.** (In EDF mode, they will survive significantly longer than in FIFO mode due to priority sorting).


# JAPANESE VER
*This project has been created as part of the 42 curriculum by amakino.*

# Codexion

## Description
「Codexion」は、限られたリソースを複数のコーダーが競い合う、マルチスレッド環境における並行処理と同期のシミュレーションです。
各コーダーはスレッドとして独立して動作し、「コンパイル」「デバッグ」「リファクタリング」のサイクルを繰り返します。量子コードのコンパイルには同時に2つのUSBドングリが必要ですが、ドングリは隣り合うコーダーと共有されています。デッドロックやリソースの枯渇（Starvation）、そしてコーダーの燃え尽き（Burnout）を防ぎながら、最適なスケジュール管理を行うことが目標です。

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
number_of_coders: コーダー（およびドングリ）の数
time_to_burnout: 最後のコンパイル開始からこのミリ秒以内に次のコンパイルを始めないとBurnoutする
time_to_compile: コンパイルにかかる時間（ミリ秒）
time_to_debug: デバッグにかかる時間（ミリ秒）
time_to_refactor: リファクタリングにかかる時間（ミリ秒）
number_of_compiles_required: 全員がこの回数コンパイルすればシミュレーション終了
dongle_cooldown: ドングリ解放後、次に使えるようになるまでの冷却時間（ミリ秒）
scheduler: キューのスケジューリングポリシー。fifo または edf（Earliest Deadline First）

#### 実行例
```bash
./codexion 5 800 200 200 200 5 10 edf
```

## Resources
[POSIX Threads (pthreads) Documentation](https://hpc-tutorials.llnl.gov/posix/)

[Mutexes and Condition Variables](https://computing.llnl.gov/tutorials/pthreads/)

[Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

[[C言語] pthread/mutexを使った並列処理](https://qiita.com/ryo_manba/items/e48faf2ba84f9e5d31c8)

[相互排他ロックの使用方法](https://docs.oracle.com/cd/E19455-01/806-2732/6jbu8v6op/index.html)

[入門者向け！C言語でのマルチスレッドをわかりやすく解説](https://daeudaeu.com/multithread/)

[マルチスレッド・プログラムの開発](https://www.ibm.com/docs/ja/aix/7.1.0?topic=programming-developing-multithreaded-programs)


#### AI Usage
本プロジェクトでは、コードの構造設計時のアイデア出し、Data Raceのチェック、およびこのREADMEのフォーマット作成においてAI（LLM）の支援を活用しています。

## Blocking cases handled
本プロジェクトでは、並行処理における以下の問題を制御・解決しています

Deadlock（デッドロック）の防止:
Coffmanの条件のうち「環状待機（Circular Wait）」を破壊するため、リソース階層（Resource Hierarchy）を導入しています。コーダーは常に「絶対IDが小さい方のドングリ」から順番にロックを取得することで、デッドロックを完全に防いでいます。

Starvation（飢餓）の防止:
リソースの奪い合いによるStarvationを防ぐため、各ドングリに独自の「優先度付きヒープキュー」を実装しました。fifo（先着順）やedf（デッドラインが近い順）のスケジューラを適用することで、公平なリソースの分配を実現しています。

ドングリのクールダウン処理:
ドングリ解放後のクールダウン時間を厳密に計算し、時間経過を条件変数を用いて待機する仕組みを導入しています。

ログのシリアライズ:
2つのメッセージが混ざることを防ぐため、出力専用のMutex（print_mutex）を使用して標準出力を保護しています。

正確なBurnout検知:
独立したモニタースレッドが数ミリ秒単位（precise_usleep(1)）で監視を行い、遅延10ms以内でBurnoutを検知して全スレッドを安全に停止させます。

## Thread synchronization mechanisms

本実装で使用しているスレッド同期のメカニズムは以下の通りです

pthread_mutex_t (ミューテックス):

dongle->mutex: 各ドングリの所有権（holder_id）や待機キュー、クールダウン情報を保護するために使用します。

state_mutex: 終了フラグ（stop_flag）や、各コーダーのコンパイル回数、最終期限（deadline）、リクエスト時間などを保護し、Data Raceを防ぎます。

print_mutex: 出力の重複を防ぐためのロック機構です。

pthread_cond_t (条件変数):

dongle->cond: ドングリが使用中、あるいはクールダウン中の場合、取得を試みるスレッドは無駄なCPUリソースを消費しないよう pthread_cond_wait で待機状態に入ります。ドングリが解放されたタイミングで pthread_cond_broadcast を呼び出し、待機中のスレッドを呼び覚ますことで、効率的なスレッド間通信を実現しています。モニターによる終了検知時にも利用され、全スレッドを安全に終了へ導きます。

# コード解説
プログラムが実行される順番に沿ってその挙動をまとめたもの

## 1. 起動と初期化フェーズ
プログラムが起動した瞬間、まず最初に行われるのは「coder」と「どんぐり」を安全に配置するための準備

### ① 引数チェックと時間の基準点
main 関数冒頭 (main.c)
引数の数が正確に 9 個（実行引数 8 個 + コマンド名）あるかチェック

init 関数 (code3_init.c) ➔ parse_arguments / validate_conf (code2_parsing.c)
引数がすべて数字であること、スケジューラ名が fifo か edf であることを確認し、構造体に値を代入

時間の基準点の確保：get_time_ms() (code4_utils.c)
gettimeofday() を使って現在のシステム時間をミリ秒精度で取得

#### なぜ「秒」と「マイクロ秒」を足し合わせるの？
tv_sec（秒）だけでは大雑把すぎ、tv_usec（マイクロ秒）だけでは1秒ごとに 0 にリセットされてしまいます。そのため、双方をミリ秒単位に揃えて（秒 $\times 1000$ ＋ マイクロ秒 $\div 1000$）合算することで、「エポック（1970年1月1日）からミリ秒単位で増え続ける、正確な1つの絶対時間」を作り出します。これを config->start_time として記録

### ② メモリの確保（一列の配列）
malloc による確保 (code3_init.c)
ドングルとコーダーの情報を格納するためのメモリスペースを確保
```c
*dongles = malloc(sizeof(t_dongle) * config->num_coders);
*coders = malloc(sizeof(t_coder) * config->num_coders);
```
この時点では、メモリ上でわは横一列にならんだ配列

### 道具の初期化と円の形成 (setup_elements / init_single_element)

一列のメモリ空間を、シミュレーションが動くための「論理的円卓」へ
#### OSへ名簿の登録（pthread_ の初期化）
pthread_mutex_init(&(dongles[i].mutex))
OSの内部に、そのドングルを独占するための「鍵管理表」を作る

pthread_cond_init(&(dongles[i].cond))
そのドングルを待つ人が寝るための待合室の名簿を作る

キュー（queue）の malloc
何人並ぶか（num_coders）は起動するまで分かりません。そのため、最大で全員が並んでも絶対に溢れないピッタリなイス（配列）を、ドングルごとに動的確保します。

巻き戻し
キューの確保に一人でも失敗した場合、逆ループを回して（while (--i >= 0)）これまでに確保したメモリを全て free して撤退します。メモリリーク対策

余り算（%）を用いた円卓の結びつけ（円卓のリンク）
```c
coders[i].left_dongle = &(dongles[i]);
coders[i].right_dongle = &(dongles[(i + 1) % config->num_coders]);
```
自分の番号 i から、次の番号 i + 1 のドングルを「人数で割った余り」で指し示すことで、配列の末尾（最後の人）の右手が自動的に先頭（0番目）のドングルを指すようになる。これにより、一列のデータが綺麗な円に

## 2. スレッドの投入と合流（幕開けと関所）
準備が整ったら、ついにプログラムはスレッドを走らせる

### ① スレッドの生成（pthread_create）
run_simulation (main.c)
監視用のスレッドを1つ、コーダー用のスレッドを人数分立ち上げます

#### &coders[i].thread_id と住所（&）を渡すりゆう
OSに対して新しいスレッドのIDを構造体に書かせるため書き込む場所の住所を渡す
pthread_create が実行された瞬間に、スレッドたちは親スレッド（メイン関数）から独立し、各自のスピードで処理を開始

### ② スレッドの合流（関所：pthread_join）
メイン関数が現場の出口で待機
もし pthread_join がないと、メイン関数はスレッドを立ち上げた直後にものすごいスピードで自身の末尾まで突っ走り、メモリを free してプログラムを強制終了させてしまいます。
pthread_join を置くことで、全員がroutineを終えて戻ってくるまで、監督はここで待つという関所の役割を果たし、プログラムの安全な後片付けを保証します。

## 3. 監視システム
裏で動く monitor_routine（code6_monitor.c）は、1ミリ秒ごとに起動する独立した監視です

### ① 全員のステータス巡回
check_coders_status は「全員仕事が終わっている（all_done = 1）」という前提を立ててから、一人ひとりを check_single_coder に送ってチェック

データレースの保護 (state_mutex)
チェック中のデータが同時に書き換えられて壊れないよう、一時的に鍵をかけます。

仕事1：生存確認（デッドライン判定）
now >= coder->deadline（現在時刻が寿命リミットを過ぎているか）を比較します。過ぎていた場合、即座に stop_flag = 1 をセットして 1（以上終了）を返します。

仕事2：ノルマ確認
コーダーのコンパイル回数が規定回数に届いていなければ、住所で渡されたフラグを「まだ終わってない奴がいる（*all_done = 0）」に書き換えます。

### ② 全員強制起床アラーム（デッドロックとフリーズの完全防止）
監視ループを抜けたとき、誰かが死んだ、または全員がノルマを終えて stop_flag = 1 になっています。
しかし、他の何人かはドングルが空くのを待って待合室（pthread_cond_wait）で深く眠っています。彼らを起こさないと、pthread_join で永遠にフリーズしてしまいます。
```c
pthread_cond_broadcast(&(coders[i].left_dongle->cond));
```
すべてのドングルを回り、「非常起床ベル（ブロードキャスト）」を一斉に鳴らします。起こされたコーダーたちは、終了合図（stop_flag）に気づき、自らスレッドを安全に終了させます。

## 4. コーダーのループとデッドロック回避
各コーダー（スレッド）は coder_routine 内で「ドングル取得 ➔ コンパイル ➔ ドングル解放 ➔ デバッグ ➔ リファクタリング」のサイクルをグルグル回ります。
### ① リソース階層化によるデッドロックのｂぷし
もし全員が「左 ➔ 右」の順にドングルを取ったら？
全員が左をキープした瞬間、右手のドングルは隣の人に握られているため、全員が譲らないまま完全にフリーズします（デッドロック）。
```c
if (coder->left_dongle->id < coder->right_dongle->id) ...
```
自分の左右にあるドングルのうち、必ずID（背番号）が小さいドングル（first）から先に取る」というルールを徹底します。
これにより、円卓の最後の1人は「右手から先に取る」という動きになり、環状の奪い合いが綺麗に断ち切られ、絶対にデッドロックしません。
### ② ドングルを1本ずつ確保：acquire_one_dongle
1,ドングルの鍵を閉め、ポインタ引き算のトリック coder - (coder->id - 1) を使ってコーダー配列の先頭アドレスを逆算します。

2,「今並んだよ」というリクエスト時刻（request_time）をミリ秒で記録します（FIFO判定用）。

3,優先度付きキュー（ヒープ木）に自身のIDを push_queue してエントリーします。

4,lock_loop に入って待機します。

### ③ 行列管理と条件待ち：lock_loop / pthread_cond_wait
ドングルを正式にゲットするための条件は3つあります。

条件1： 誰も使っていない（holder_id == 0）

条件2： 行列の先頭が自分である（queue[0] == coder->id）

条件3： ドングルのクールダウンが完了している

自分の番だけどクールダウン中の場合：
無駄なロック競合を避けるため、一旦ドングルの鍵を開けてから精密に眠ります（unlock ➔ sleep ➔ lock）。

自分の番ではない場合：
pthread_cond_wait で待合室のベッドに入り、他の人が使い終わるまで眠りにつきます（CPU使用率 0%）。

#### cond_wait から起きたあとの動き
アラーム（broadcast）が鳴って目が覚めたとき、スレッドは即座に起き上がれるわけではありません。
OSのルール上、「手放した鍵（ミューテックス）をもう一度獲得し直す（ロックを奪い返す）」まで、関数の中に閉じ込められたまま闘います。
鍵を奪い返して無事に関数から脱出すると、ループの最初に戻り、本当に自分が先頭なのか（誰かに追い抜かれていないか）を厳密にチェックし直します。

### ④ free：release_dongles
コンパイルが終わったら、デバッグやリファクタリング（これらにはドングルは不要）に入る前に、最速で release_dongles を呼び出します。
holder_idを0に戻し、次の人のために起床ベル（broadcast）を鳴らし、ドングルが再び使えるようになるまでの「クールダウン終了時間」を設定します。これにより、両隣のコーダーの燃え尽きを完璧に防ぎます。

## 5. スケジューラ（順序の決定）：is_higher_priority
複数のスレッドが同じドングルを求めて列に並ぶとき、誰を前にするべきか（優先度が高いか）を判断する「審判」がこの関数です（code7_queue.c）。

FIFO（早い者勝ち）の場合：
request_time（並んだ時刻）がより過去（小さい数字）であるコーダーの勝ち。

EDF（締め切り優先）の場合：
deadline（燃え尽き時刻）がより直近（小さい数字）であるコーダーの勝ち。

同点時のタイブレーカー（Tie-breaker）：
ミリ秒まで完全に同点だった場合は、「IDが小さい方を優先する（a->id < b->id）」 というルールにより、プログラムの挙動が運任せになるのを防ぎ、再現性を担保しています。


## 燃え尽き防止の数理モデル（生存・飢餓）redditの投稿を参考にしたので、間違ってたらスマソ

与えられた入力パラメータがシミュレーション上安全なのか、それとも不可避な飢餓（スターベーション）や燃え尽きを引き起こすのかを判定するため、コーダーの1サイクルの行動を以下の数式モデルで定義:

$$T_{\text{cycle}} = t_{\text{compile}} + t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}} + W$$

ここで $W$ は、左右の両隣にある共有ドングルが空くのを待つ「待機時間」を指します。この1サイクル全体の合計時間が寿命リミットを上回る、すなわち $T_{\text{cycle}} > t_{\text{burnout}}$ となったとき、コーダーは構造的に **燃え尽き（Burnout）** を迎えます。

### 待機時間 ($W$) の境界条件
* **最良のシナリオ ($W_{\text{min}}$):** リファクタリングが終了した瞬間、左右のドングルが両方とも完全にフリーな状態（$W_{\text{min}} = 0$）。
* **最悪のシナリオ ($W_{\text{max}}$):** 円卓におけるリソース共有の構造上、最悪待機時間の天井は全体の人数（`number_of_coders`）が**偶数**か**奇数**かによって大きく変動します。

#### ケース 1: コーダーが偶数人の場合
人数が偶数の場合、コーダーたちは綺麗に対称的なグループに分かれて交互にドングルを回すことができます（例：4人の場合、まず1番と3番が同時にコンパイルし、次に2番と4番が同時にコンパイルする、という並行サイクル）。
* このため、最悪の待ち時間 $W{\text{max}}$ は、隣のコーダー1人分の**「1回のコンパイル時間 ＋ 1回のクールダウン時間」**に抑えられます。
* **最悪値のサイクル公式:**
$$T_{\text{cycle-max}} = 2 \times t_{\text{compile}} + 2 \times t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}}$$
* $t_{\text{burnout}}$ がこの $T_{\text{cycle-max}}$ より大きく設定されていれば、理論上飢餓は発生せず全員生存可能です。

#### ケース 2: コーダーが奇数人の場合
人数が奇数の場合、完全に交互に並ぶ対称性が崩れるため、待機タイミングが互い違い（千鳥足状）のウェイトパターンになります。
* 最悪のケースでは、自分の左右にいる **両隣のコーダー2人** のコンパイル処理が連続して重なり、それらが順番に終わるのを丸々待たされることになります。つまり、 **「2回のコンパイル時間 ＋ 2回のクールダウン時間」** の待機ペナルティが発生します。
* **最悪値のサイクル公式:**
$$T_{\text{cycle-max}} = 3 \times t_{\text{compile}} + 3 \times t_{\text{cooldown}} + t_{\text{debug}} + t_{\text{refactor}}$$
* 奇数人のシミュレーションで生存を担保するためには、偶数人のときよりも大幅に大きな $t_{\text{burnout}}$ のマージン（余裕）が必要になります。

### 📋 評価用・その場でできる生存判定ワークシート

レビュワーは、テストコマンドの数字を以下の公式に当てはめることで、そのケースが数学的に **「全員生き残るべき設定」 **なのか、それとも**「燃え尽き（Burnout）が発生して正常な設定」** なのかをその場で即座に判定できます。

#### ステップ 1: コマンドの引数から数値を抽出する
実行された以下のコマンドから数値を抜き出します：
`./codexion [num_coders] [time_burnout] [time_compile] [time_debug] [time_refactor] [num_req] [cooldown] [scheduler]`

* $N$ = `num_coders`（人数）
* $T_{\text{burnout}}$ = `time_burnout`（燃え尽き時間）
* $t_{\text{comp}}$ = `time_compile`（コンパイル時間）
* $t_{\text{cool}}$ = `dongle_cooldown`（クールダウン）
* $t_{\text{dbg}}$ = `time_debug`（デバッグ時間）
* $t_{\text{ref}}$ = `time_refactor`（リファクタ時間）

#### ステップ 2: 人数（$N$）に応じて計算式を選ぶ

* **人数 $N$ が『偶数』の場合 (2, 4, 6, 8...):**
$$T_{\text{cycle-max}} = (2 \times t_{\text{comp}}) + (2 \times t_{\text{cool}}) + t_{\text{dbg}} + t_{\text{ref}}$$

* **人数 $N$ が『奇数』の場合 (3, 5, 7, 9...):**
$$T_{\text{cycle-max}} = (3 \times t_{\text{comp}}) + (3 \times t_{\text{cool}}) + t_{\text{dbg}} + t_{\text{ref}}$$

#### ステップ 3: 計算結果と入力値を比較して判定する
算出した最悪サイクル時間（$T_{\text{cycle-max}}$）と、引数の $T_{\text{burnout}}$ を比較します：

* 🟢 **$T_{\text{burnout}} > T_{\text{cycle-max}}$ の場合 :** 
  理論上、リソースの割り当て時間に十分な猶予があります。 **【正常な挙動】全員が一度も燃え尽きることなくノルマを達成して終了します。** （もしこれで誰かが燃え尽きる場合、スレッドの同期漏れやデータレースのバグが存在します）
* 🔴 **$T_{\text{burnout}} \le T_{\text{cycle-max}}$ の場合 :** 
  リソースの奪い合いによって誰かが確実に制限時間をオーバーします。**【正常な挙動】高い確率で誰か1人が `burned out` してシミュレーションが停止します。**（なお、EDFモードはFIFOモードに比べて優先度制御が働くため、燃え尽きるまでの生存時間が大幅に伸びます）。

# tester

１，全員生存・正常終了

検証内容: 200人以下、60ms以上。誰も燃え尽きず規定回数をクリアして安全に終わるか。寿命に圧倒的なマージンを持たせる
./codexion 4 1200 200 100 100 5 0 edf
./codexion 15 2000 200 150 150 5 20 edf
./codexion 4 2000 200 100 100 10 0 edf

２Burnout検知と10msの精度
検証内容: 確実に誰かを燃え尽きさせ、モニターがデッドラインから10ms以内に正確にログを出してシステムを緊急停止できるか。絶対にクリア不可能
./codexion 1 500 200 100 100 5 0 fifo
./codexion 3 300 200 100 100 5 10 fifo / ※最初の2人が200msコンパイルしている間に、あぶれた1人が300msで確実に餓死

3. Codexion testing — Medium: Cooldown behavior（クールダウンの検証）
検証内容: ドングル解放後のクールダウン時間が厳密に守られるか
./codexion 4 1500 200 100 100 5 200 fifo クリア不能
./codexion 4 3500 200 100 100 5 200 fifo


4. Codexion testing — Medium: Scheduler differences
【要件】 FIFOだと確実に死ぬが、EDFだとキューの並び替えによって「絶対に全員生存」する
./codexion 4 400 100 100 100 15 20 fifo
./codexion 4 400 100 100 100 15 20 edf

```bash
#!/bin/bash

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
BLUE="\033[0;34m"
RESET="\033[0m"


echo -e "${CYAN}==================================================${RESET}"
echo -e "${CYAN}       Codexion Automated Test Suite            ${RESET}"
echo -e "${CYAN}==================================================${RESET}"

echo -e "${YELLOW}[1/2] 最新のコードでビルド中... (make re)${RESET}"
make re > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}Error: コンパイルに失敗しました。コードを確認してください。${RESET}"
    exit 1
fi
echo -e "${GREEN}➔ ビルド成功！${RESET}\n"

function wait_user() {
    echo -e "${BLUE}--------------------------------------------------${RESET}"
    read -p "エンターキーを押すと次のテストを実行します..."
    clear
}

# -----------------------------------------------------------------
# TEST 1: Easy Test (Normal Execution)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 1] Easy Test: 通常実行（全員生存・ノルマ達成終了）${RESET}"
echo -e "${YELLOW}Description:${RESET} 全員が燃え尽きることなく5回コンパイルして安全に終了するか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 4 1000 200 200 200 5 0 fifo"
echo -e "${MAGENTA}Expected:${RESET} 全員が5回ずつコンパイルし、Burnoutのログなしで即座にプロンプトに戻ること。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 4 1000 200 200 200 5 0 fifo
wait_user

# -----------------------------------------------------------------
# TEST 2: Cooldown Handling
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 2] Medium Test: クールダウン処理の検証${RESET}"
echo -e "${YELLOW}Description:${RESET} ドングル解放後の100msの冷却時間が守られ、スタベーションが起きないか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 4 1200 200 200 200 5 100 fifo"
echo -e "${MAGENTA}Expected:${RESET} クールダウンを挟みつつも、寿命(1200ms)に余裕があるため全員無事にノルマを達成すること。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 4 1200 200 200 200 5 100 fifo
wait_user

# -----------------------------------------------------------------
# TEST 3: Burnout Detection (Less Easy)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 3] Less Easy: Burnout（燃え尽き）検知と即時停止${RESET}"
echo -e "${YELLOW}Description:${RESET} 奇数人数かつ厳しい時間設定で、正確に燃え尽きを検知して10ms以内に全スレッドが止まるか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 400 200 100 100 10 0 fifo"
echo -e "${MAGENTA}Expected:${RESET} 誰か1人が 'burned out' と出た瞬間に全体のログがピタッと止まり、フリーズせずに終了すること。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 3 400 200 100 100 10 0 fifo
wait_user

# -----------------------------------------------------------------
# TEST 4: Scheduler Difference - Part 1 (FIFO / Failure)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 4-1] Medium Test: FIFOでの限界限界テスト（燃え尽き発生）${RESET}"
echo -e "${YELLOW}Description:${RESET} 先着順（FIFO）では、命が危ない奴を優先できないため燃え尽きる現象を確認します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 600 200 100 100 10 50 fifo"
echo -e "${MAGENTA}Expected:${RESET} 優先度制御がないため、高確率で誰かが途中で 'burned out' して終了します。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 3 600 200 100 100 10 50 fifo
wait_user

# -----------------------------------------------------------------
# TEST 5: Scheduler Difference - Part 2 (EDF / Success)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 4-2] Medium Test: EDFでの限界限界テスト（全員生存）${RESET}"
echo -e "${YELLOW}Description:${RESET} 全く同じ引数ですが、EDF（締め切り優先）にすることで、ヒープ木が命の危険を察知してごぼう抜き延命させるのを確認します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 600 200 100 100 10 50 edf"
echo -e "${MAGENTA}Expected:${RESET} 【ウチのヒープ木の真骨頂】さっきは死んだ設定なのに、今度は全員が最後まで生き残りノルマを完遂します！"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 3 600 200 100 100 10 50 edf
wait_user

# -----------------------------------------------------------------
# TEST 6: Overflow Validation
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 5] Edge Case: 超巨大な数字の引数（オーバーフロー防御）${RESET}"
echo -e "${YELLOW}Description:${RESET} intの限界を超えたアホみたいな数字が渡された際、バリデーションで安全に弾けるか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 2 1200 200 200 200 2 10000000000000000000000000000000000000000000 edf"
echo -e "${MAGENTA}Expected:${RESET} 未定義動作で突き進むのではなく、即座に 'Error: arg 7 must be numeric' 等のエラーを出して安全に終了すること。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 2 1200 200 200 200 2 10000000000000000000000000000000000000000000 edf
wait_user

# -----------------------------------------------------------------
# TEST 7: Edge Case (1 Coder)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 6] Edge Case: コーダーが1人しかいない場合${RESET}"
echo -e "${YELLOW}Description:${RESET} コーダーが1人の時、ドングルも1本しかありません。コンパイルには2本必要なので、1本キープしたまま安全に待機してBurnoutするか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 1 1000 200 200 200 2 0 edf"
echo -e "${MAGENTA}Expected:${RESET} 1本確保（has taken a dongle）した状態で安全にフリーズを回避し、1000ms後に正確に 'burned out' して安全に終了すること。"
echo -e "${BLUE}--- 実行開始 ---${RESET}"
./codexion 1 1000 200 200 200 2 0 edf

echo -e "\n${GREEN}==================================================${RESET}"
echo -e "${GREEN}         すべてのテストケースが完了しました！        ${RESET}"
echo -e "${GREEN}==================================================${RESET}"
```

save to txt
```shell
#!/bin/bash

RESULT_FILE="test_results.txt"


GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
BLUE="\033[0;34m"
MAGENTA="\033[0;35m"
RESET="\033[0m"

clear
echo -e "${CYAN}==================================================${RESET}"
echo -e "${CYAN}       Codexion Automated Test & Logging          ${RESET}"
echo -e "${CYAN}==================================================${RESET}"

echo "=== Codexion Test Results ===" > $RESULT_FILE
echo "Generated on: $(date)" >> $RESULT_FILE
echo -e "➔ ログファイル ${YELLOW}${RESULT_FILE}${RESET} を初期化しました。"

echo -e "${YELLOW}[1/2] 最新のコードでビルド中... (make re)${RESET}"
make re > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}Error: コンパイルに失敗しました。${RESET}"
    exit 1
fi
echo -e "${GREEN}➔ ビルド成功！${RESET}\n"

function wait_user() {
    echo -e "${BLUE}--------------------------------------------------${RESET}"
    read -p "エンターキーを押すと次のテストを実行します..."
    clear
}


function log_title() {
    echo -e "\n==================================================" >> $RESULT_FILE
    echo "  $1" >> $RESULT_FILE
    echo "==================================================" >> $RESULT_FILE
}

# -----------------------------------------------------------------
# TEST 1: Easy Test (Normal Execution)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 1] Easy Test: 通常実行（全員生存・ノルマ達成終了）${RESET}"
echo -e "${YELLOW}Description:${RESET} 全員が燃え尽きることなく5回コンパイルして安全に終了するか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 4 1000 200 200 200 5 0 fifo"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 1: ./codexion 4 1000 200 200 200 5 0 fifo"
# tee -a で画面に出しつつファイルにも追記
./codexion 4 1000 200 200 200 5 0 fifo 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 2: Cooldown Handling
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 2] Medium Test: クールダウン処理の検証${RESET}"
echo -e "${YELLOW}Description:${RESET} ドングル解放後の100msの冷却時間が守られ、スタベーションが起きないか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 4 1200 200 200 200 5 100 fifo"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 2: ./codexion 4 1200 200 200 200 5 100 fifo"
./codexion 4 1200 200 200 200 5 100 fifo 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 3: Burnout Detection (Less Easy)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 3] Less Easy: Burnout（燃え尽き）検知と即時停止${RESET}"
echo -e "${YELLOW}Description:${RESET} 正確に燃え尽きを検知して10ms以内に全スレッドが止まるか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 400 200 100 100 10 0 fifo"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 3: ./codexion 3 400 200 100 100 10 0 fifo"
./codexion 3 400 200 100 100 10 0 fifo 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 4: Scheduler Difference - Part 1 (FIFO / Failure)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 4-1] Medium Test: FIFOでの限界テスト（燃え尽き発生）${RESET}"
echo -e "${YELLOW}Description:${RESET} 先着順（FIFO）では、命が危ない奴を優先できないため燃え尽きる現象を確認します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 600 200 100 100 10 50 fifo"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 4-1: ./codexion 3 600 200 100 100 10 50 fifo"
./codexion 3 600 200 100 100 10 50 fifo 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 5: Scheduler Difference - Part 2 (EDF / Success)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 4-2] Medium Test: EDFでの限界テスト（全員生存）${RESET}"
echo -e "${YELLOW}Description:${RESET} EDF（締め切り優先）にすることで、ヒープ木が命の危険を察知してごぼう抜き延命させるのを確認します。"
echo -e "${CYAN}Command:${RESET} ./codexion 3 600 200 100 100 10 50 edf"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 4-2: ./codexion 3 600 200 100 100 10 50 edf"
./codexion 3 600 200 100 100 10 50 edf 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 6: Overflow Validation
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 5] Edge Case: 超巨大な数字の引数（オーバーフロー防御）${RESET}"
echo -e "${YELLOW}Description:${RESET} intの限界を超えた数字が渡された際、バリデーションで安全に弾けるか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 2 1200 200 200 200 2 10000000000000000000000000000000000000000000 edf"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 5: Overflow case"
./codexion 2 1200 200 200 200 2 10000000000000000000000000000000000000000000 edf 2>&1 | tee -a $RESULT_FILE

wait_user

# -----------------------------------------------------------------
# TEST 7: Edge Case (1 Coder)
# -----------------------------------------------------------------
echo -e "${GREEN}[TEST 6] Edge Case: コーダーが1人しかいない場合${RESET}"
echo -e "${YELLOW}Description:${RESET} 1本確保（has taken a dongle）した状態で安全にフリーズを回避し、1000ms後に正確にburned outするか検証します。"
echo -e "${CYAN}Command:${RESET} ./codexion 1 1000 200 200 200 2 0 edf"
echo -e "${BLUE}--- 実行開始 ---${RESET}"

log_title "TEST 6: ./codexion 1 1000 200 200 200 2 0 edf"
./codexion 1 1000 200 200 200 2 0 edf 2>&1 | tee -a $RESULT_FILE

echo -e "\n${GREEN}==================================================${RESET}"
echo -e "${GREEN}         すべてのテストケースが完了しました！        ${RESET}"
echo -e "${GREEN}➔ ログが ${YELLOW}${RESULT_FILE}${RESET} に自動保存されました。${RESET}"
echo -e "${GREEN}==================================================${RESET}"
```
