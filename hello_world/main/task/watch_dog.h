/// @brief 一直占用cpu 触发 IDL 5s 超时
/// @param pvParam 
void task_watch_dog_1(void *pvParam);

/// @brief 通过Delay 让出cpu, 避免触发 IDL 超时
/// @param pvParam 
void task_watch_dog_2(void *pvParam);

/// @brief 将自己加入看门狗，但是不喂狗
/// @param pvParam 
void task_watch_dog_3(void *pvParam);

/// @brief 将自己加入看门狗，定时喂狗
/// @param pvParam 
void task_watch_dog_4(void *pvParam);

void test_watch_dog_case_1();

void test_watch_dog_case_2();

void test_watch_dog_case_3();

void test_watch_dog_case_4();