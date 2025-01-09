#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 宏定义，用于表示学生信息结构体中姓名的最大长度
#define MAX_NAME_LENGTH 50

// 枚举类型，定义学生的性别
enum Gender
{
    MALE,
    FEMALE
};

// 定义联合体，用于存储不同类型的学号（假设可以是整数或者字符串形式）
union StudentID
{
    int idInt;
    char idString[20];
};

// 定义学生信息结构体
typedef struct Student
{
    char name[MAX_NAME_LENGTH];
    int age;
    enum Gender gender;
    float score;
    union StudentID studentID;    // 包含联合体成员，用于学号
    long registrationTime;        // 注册时间，使用长整型表示时间戳（简单示意）
    double scholarship;           // 奖学金金额，使用双精度浮点数
    unsigned int attendanceCount; // 出勤次数，使用无符号整数
} Student;

// 函数声明，用于录入单个学生信息
Student inputStudentInfo();

// 函数声明，用于将学生信息保存到文件
void saveStudentsToFile(Student *students, int numStudents, const char *filename);

// 函数声明，用于从文件读取学生信息
Student *readStudentsFromFile(int *numStudents, const char *filename);

// 函数声明，用于计算学生的平均成绩
float calculateAverageScore(Student *students, int numStudents);

// 函数声明，用于统计出勤次数高于某个阈值的学生数量
int countStudentsWithHighAttendance(Student *students, int numStudents, unsigned int threshold);

// 函数声明，用于使用位运算设置学生成绩的及格/不及格标志（简单示例，假设60分为及格）
void setPassFlag(Student *students, int numStudents);

// 函数声明，用于验证输入是否在指定范围内（辅助函数）
int validateInputRange(int input, int min, int max);

int main()
{
    int choice;
    Student *students = NULL;
    int numStudents = 0;

    do
    {
        // 显示菜单
        printf("1. 录入学生信息\n");
        printf("2. 保存学生信息到文件\n");
        printf("3. 从文件读取学生信息\n");
        printf("4. 计算学生平均成绩\n");
        printf("5. 统计出勤情况良好的学生数量\n");
        printf("6. 设置学生成绩及格/不及格标志\n");
        printf("7. 退出\n");
        printf("请输入你的选择：");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            Student newStudent = inputStudentInfo();
            // 动态内存分配，重新分配空间以容纳新录入的学生信息
            students = (Student *)realloc(students, (numStudents + 1) * sizeof(Student));
            if (students == NULL)
            {
                perror("内存分配失败");
                exit(EXIT_FAILURE);
            }
            students[numStudents] = newStudent;
            numStudents++;
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
        {
            float averageScore = calculateAverageScore(students, numStudents);
            printf("学生平均成绩为：%.2f\n", averageScore);
            break;
        }
        case 5:
        {
            unsigned int attendanceThreshold;
            printf("请输入出勤次数阈值：");
            // 使用do-while循环进行输入验证，确保输入是合理的正整数
            do
            {
                scanf("%u", &attendanceThreshold);
                if (!validateInputRange(attendanceThreshold, 0, 100))
                {
                    printf("输入的出勤次数阈值不合理，请重新输入：");
                }
            } while (!validateInputRange(attendanceThreshold, 0, 100));

            int count = countStudentsWithHighAttendance(students, numStudents, attendanceThreshold);
            printf("出勤次数高于 %u 的学生数量为：%d\n", attendanceThreshold, count);
            break;
        }
        case 6:
            setPassFlag(students, numStudents);
            break;
        case 7:
            printf("退出程序\n");
            goto CLEAN_UP; // 使用goto语句跳转到内存释放的代码段，用于程序退出前清理资源
        default:
            printf("无效的选择，请重新输入\n");
        }

    } while (1);

CLEAN_UP:
    // 释放动态分配的内存
    if (students != NULL)
    {
        free(students);
    }
    return 0;
}

// 录入单个学生信息的函数实现
Student inputStudentInfo()
{
    Student student;
    printf("请输入学生姓名：");
    scanf("%s", student.name);
    printf("请输入学生年龄：");
    // 使用do-while循环验证年龄输入是否在合理范围（比如1到100岁之间）
    do
    {
        scanf("%d", &student.age);
        if (!validateInputRange(student.age, 1, 100))
        {
            printf("输入的年龄不合理，请重新输入：");
        }
    } while (!validateInputRange(student.age, 1, 100));
    printf("请输入学生性别(0表示男性,1表示女性):");
    int genderChoice;
    scanf("%d", &genderChoice);
    student.gender = (enum Gender)genderChoice;
    printf("请输入学生成绩：");
    scanf("%f", &student.score);
    printf("请输入学号（整数形式输入，按回车键后若想以字符串形式输入请再次输入）：");
    // 先尝试以整数形式读取学号
    if (scanf("%d", &student.studentID.idInt) == 1)
    {
        // 如果成功读取整数，说明学号以整数形式输入
    }
    else
    {
        // 如果读取整数失败，清空输入缓冲区，再以字符串形式读取学号
        while (getchar() != '\n')
            ;
        scanf("%s", student.studentID.idString);
    }
    printf("请输入注册时间（时间戳，长整型）：");
    scanf("%ld", &student.registrationTime);
    printf("请输入奖学金金额：");
    scanf("%lf", &student.scholarship);
    printf("请输入出勤次数：");
    scanf("%u", &student.attendanceCount);
    return student;
}

// 将学生信息保存到文件的函数实现
void saveStudentsToFile(Student *students, int numStudents, const char *filename)
{
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
        fprintf(fp, "%ld %.2lf %u\n", students[i].registrationTime, students[i].scholarship, students[i].attendanceCount);
    }
    fclose(fp);
    printf("学生信息已成功保存到文件 %s\n", filename);
}

// 从文件读取学生信息的函数实现
Student *readStudentsFromFile(int *numStudents, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    Student *students = NULL;
    int capacity = 10; // 初始分配空间可容纳10个学生信息
    students = (Student *)malloc(capacity * sizeof(Student));
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
            students = (Student *)realloc(students, capacity * sizeof(Student));
            if (students == NULL)
            {
                perror("内存分配失败");
                fclose(fp);
                exit(EXIT_FAILURE);
            }
        }
        if (fscanf(fp, "%s %d %d %f", students[*numStudents].name, &students[*numStudents].age,
                   &students[*numStudents].gender, &students[*numStudents].score) == 4)
        {
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
            fscanf(fp, "%ld %lf %u", &students[*numStudents].registrationTime, &students[*numStudents].scholarship,
                   &students[*numStudents].attendanceCount);
            (*numStudents)++;
        }
    }
    fclose(fp);
    return students;
}

// 计算学生平均成绩的函数实现
float calculateAverageScore(Student *students, int numStudents)
{
    if (numStudents == 0)
    {
        return 0;
    }
    float sum = 0;
    for (int i = 0; i < numStudents; i++)
    {
        sum += students[i].score;
    }
    return sum / numStudents;
}

// 统计出勤次数高于某个阈值的学生数量的函数实现
int countStudentsWithHighAttendance(Student *students, int numStudents, unsigned int threshold)
{
    int count = 0;
    for (int i = 0; i < numStudents; i++)
    {
        if (students[i].attendanceCount > threshold)
        {
            count++;
        }
    }
    return count;
}

// 使用位运算设置学生成绩及格/不及格标志的函数实现（简单示例）
void setPassFlag(Student *students, int numStudents)
{
    for (int i = 0; i < numStudents; i++)
    {
        int passFlag = (students[i].score >= 60);                               // 转换为0或1表示是否及格
        students[i].score = ( students[i].score & 0xFFFFFF00) | (passFlag << 0); // 假设成绩用float存储，这里简单利用位运算将最低位置为及格标志
    }
    printf("已设置学生成绩的及格/不及格标志\n");
}

// 验证输入是否在指定范围内（辅助函数）
int validateInputRange(int input, int min, int max)
{
    return (input >= min && input <= max);
}