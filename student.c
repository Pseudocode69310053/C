
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>

// 定义学生信息结构体中姓名的最大长度
#define MAX_NAME_LENGTH 50

// 定义安全的输入缓冲区大小（避免缓冲区溢出）
#define INPUT_BUFFER_SIZE 100

// 定义学生的性别枚举类型
typedef enum
{
    MALE,
    FEMALE
} Gender;

// 定义联合体，用于存储不同类型的学号（假设可以是整数或者字符串形式）
typedef union
{
    int idInt;
    char idString[20];
} StudentID;

// 定义学生信息结构体
typedef struct Student
{
    char name[MAX_NAME_LENGTH];
    int age;
    Gender gender;
    float score;
    StudentID studentID;
    long registrationTime;
} Student;

// 函数声明，用于录入单个学生信息
Student inputStudentInfo();
// 函数声明，用于将学生信息保存到文件
void saveStudentsToFile(Student *students, int numStudents, const char *filename);
// 函数声明，用于从文件读取学生信息
Student *readStudentsFromFile(int *numStudents, const char *filename);
// 信号处理函数，用于处理特定信号（这里是SIGINT）
void signal_handler(int signum);
// 可变参数函数声明，用于打印格式化的调试信息（示例）
void debugPrint(const char *format, ...);

int main()
{
    // 注册信号处理函数，当接收到SIGINT信号（如按下Ctrl + C）时调用signal_handler函数
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("注册信号处理函数失败");
        return 1;
    }

    int choice;
    Student *students = NULL;
    int numStudents = 0;

    do
    {
        // 显示菜单
        printf("1. 录入学生信息\n");
        printf("2. 保存学生信息到文件\n");
        printf("3. 从文件读取学生信息\n");
        printf("4. 退出\n");
        printf("请输入你的选择：");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            Student newStudent = inputStudentInfo();
            // 动态内存分配，重新分配空间以容纳新录入的学生信息
            students = realloc(students, (numStudents + 1) * sizeof(Student));
            if (students == NULL)
            {
                perror("内存分配失败");
                exit(EXIT_FAILURE);
            }
            students[numStudents++] = newStudent;
            break;
        }
        case 2:
            saveStudentsToFile(students, numStudents, "students.txt");
            break;
        case 3:
            students = readStudentsFromFile(&numStudents, "students.txt");
            if (students == NULL)
            {
                printf("读取文件失败或文件中无数据\n");
            }
            break;
        case 4:
            printf("退出程序\n");
            break;
        default:
            printf("无效的选择，请重新输入\n");
        }

    } while (choice != 4);

    // 释放动态分配的内存
    if (students != NULL)
    {
        free(students);
    }
    return 0;
}

// 录入单个学生信息的函数实现，使用更安全的输入函数并添加错误处理
Student inputStudentInfo()
{
    Student student;
    char buffer[INPUT_BUFFER_SIZE]; // 使用缓冲区避免输入溢出

    // 输入学生姓名，使用fgets替代scanf避免缓冲区溢出问题
    printf("请输入学生姓名：");
    fgets(buffer, INPUT_BUFFER_SIZE, stdin);
    // 移除fgets可能读取到的换行符
    buffer[strcspn(buffer, "\n")] = '\0';
    strncpy(student.name, buffer, MAX_NAME_LENGTH - 1);
    student.name[MAX_NAME_LENGTH - 1] = '\0';

    // 输入学生年龄并验证范围
    int age;
    do
    {
        printf("请输入学生年龄：");
        if (scanf("%d", &age) != 1)
        {
            // 处理输入非整数的错误情况，清空输入缓冲区
            while (getchar() != '\n')
                ;
            printf("输入的年龄格式不正确，请重新输入。\n");
            continue;
        }
        if (age < 1 || age > 100)
        {
            printf("年龄输入不合理，请重新输入：");
        }
    } while (age < 1 || age > 100);
    student.age = age;

    // 输入学生性别并进行类型检查
    int genderInput;
    do
    {
        printf("请输入学生性别(0表示男性,1表示女性):");
        if (scanf("%d", &genderInput) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("输入的性别格式不正确，请重新输入。\n");
            continue;
        }
        if (genderInput != 0 && genderInput != 1)
        {
            printf("性别输入不合理，请重新输入：");
        }
    } while (genderInput != 0 && genderInput != 1);
    student.gender = (Gender)genderInput; // 强制类型转换，将输入的整数转换为Gender枚举类型

    // 输入学生成绩
    printf("请输入学生成绩：");
    if (scanf("%f", &student.score) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("输入的成绩格式不正确，请重新输入。\n");
    }

    // 输入学号，先尝试以整数形式读取学号
    printf("请输入学号（整数形式输入，按回车键后若想以字符串形式输入请再次输入）：");
    if (scanf("%d", &student.studentID.idInt) != 1)
    {
        // 如果读取整数失败，清空输入缓冲区，再以字符串形式读取学号
        while (getchar() != '\n')
            ;
        scanf("%s", student.studentID.idString);
    }

    // 输入注册时间（时间戳，长整型）
    printf("请输入注册时间（时间戳，长整型）：");
    if (scanf("%ld", &student.registrationTime) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("输入的注册时间格式不正确，请重新输入。\n");
    }

    // 使用静态局部变量记录当前录入学生信息的次数（仅作示例，可按需扩展功能）
    static int input_count = 0;
    input_count++;
    printf("这是第 %d 次录入学生信息\n", input_count);

    return student;
}

// 将学生信息保存到文件的函数实现，添加错误处理宏和更多调试信息输出
void saveStudentsToFile(Student *students, int numStudents, const char *filename)
{
#ifndef DEBUG
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("打开文件失败");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < numStudents; i++)
    {
        fprintf(fp, "%s %d %d %.2f ", students[i].name, students[i].age, students[i].gender, students[i].score);
        if (students[i].studentID.idInt != 0)
        {
            fprintf(fp, "%d ", students[i].studentID.idInt);
        }
        else
        {
            fprintf(fp, "%s ", students[i].studentID.idString);
        }
        fprintf(fp, "%ld\n", students[i].registrationTime);
    }
    fclose(fp);
    printf("学生信息已成功保存到文件 %s\n", filename);
#else
    // 如果定义了DEBUG宏，在调试模式下使用可变参数函数输出详细调试信息（模拟调试行为）
    debugPrint("在调试模式下，不会实际保存文件，假装已将学生信息保存到文件 %s\n", filename);
#endif
}

// 从文件读取学生信息的函数实现，完善错误处理和对未定义行为的避免
Student *readStudentsFromFile(int *numStudents, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    Student *students = NULL;
    int capacity = 10; // 初始分配空间可容纳10个学生信息
    students = malloc(capacity * sizeof(Student));
    if (students == NULL)
    {
        perror("内存分配失败");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    *numStudents = 0;
    while (!feof(fp))
    {
        if (*numStudents >= capacity)
        {
            // 空间不足时，动态扩容
            capacity *= 2;
            students = realloc(students, capacity * sizeof(Student));
            if (students == NULL)
            {
                perror("内存分配失败");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
        }
        char nameBuffer[MAX_NAME_LENGTH];
        int age;
        int gender;
        float score;
        // 避免使用fscanf直接读取结构体成员，先读取到临时变量，避免因格式不匹配导致的未定义行为
        if (fscanf(fp, "%s %d %d %f", nameBuffer, &age, &gender, &score) == 4)
        {
            strncpy(students[*numStudents].name, nameBuffer, MAX_NAME_LENGTH - 1);
            students[*numStudents].name[MAX_NAME_LENGTH - 1] = '\0';
            students[*numStudents].age = age;
            students[*numStudents].gender = (Gender)gender; // 强制类型转换
            students[*numStudents].score = score;

            // 尝试读取学号（先按整数读取）
            if (fscanf(fp, "%d", &students[*numStudents].studentID.idInt) == 1)
            {
                // 如果成功读取整数，说明学号以整数形式存储
            }
            else
            {
                // 如果读取整数失败，说明学号可能是字符串形式，回退文件指针后按字符串读取
                fseek(fp, -1 * (int)sizeof(int), SEEK_CUR);
                fscanf(fp, "%s", students[*numStudents].studentID.idString);
            }
            if (fscanf(fp, "%ld", &students[*numStudents].registrationTime) != 1)
            {
                // 处理注册时间读取失败的情况，设置一个默认值或者输出错误信息等
                students[*numStudents].registrationTime = 0;
                // 可以添加更多错误处理逻辑，比如记录错误日志等
                debugPrint("读取注册时间失败,设置默认值为0\n");
            }
            (*numStudents)++;
        }
    }
    fclose(fp);
    return students;
}

// 信号处理函数，用于处理SIGINT信号（这里简单打印提示信息并正常退出程序）
void signal_handler(int signum)
{
    printf("接收到中断信号(Ctrl + C)正在安全退出程序...\n");
    // 释放已经分配的内存（假设全局的students指针指向了有效内存）
    if (students != NULL)
    {
        free(students);
    }
    exit(0);
}

// 可变参数函数实现，用于打印格式化的调试信息（示例）
void debugPrint(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args); // 输出到标准错误输出，更适合调试信息
    va_end(args);
}