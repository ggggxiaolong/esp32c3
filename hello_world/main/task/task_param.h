void task_param_int(void *pvParam);
void task_param_arr(void *pvParam);
void task_param_struct(void *pvParam);
void task_param_string(void *pvParam);

typedef struct transParam
{
    int param1;
    int param2;
} TransParam;

void test_task_param_case_1();
void test_task_param_case_2();
void test_task_param_case_3();
void test_task_param_case_4();