#include <stdio.h>

#define MAX_SCHEDULES 100

typedef enum
{
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
    Invalid
} DayOfWeek;

// スケジュール構造体の定義
typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    char description[100];
    DayOfWeek dayOfWeek;
} Schedule;

// 関数プロトタイプ
void loadSchedules(Schedule schedules[], int *count, const char *filename);
void saveSchedules(Schedule schedules[], int count, const char *filename);
void addSchedule(Schedule schedules[], int *count);
void displaySchedules(Schedule schedules[], int count);
void deleteSchedule(Schedule schedules[], int *count);
int compareSchedules(const Schedule *a, const Schedule *b);
void sortSchedules(Schedule schedules[], int count);
int isValidDate(int year, int month, int day, int hour, int minute);
DayOfWeek calculateDayOfWeek(int year, int month, int day);
const char *getDayOfWeekString(DayOfWeek day);

// メイン関数
int main()
{
    Schedule schedules[MAX_SCHEDULES];
    int scheduleCount = 0;
    const char *filename = "schedules.txt";

    loadSchedules(schedules, &scheduleCount, filename);

    int choice;
    do
    {
        printf("\n--- スケジュール管理 ---\n");
        printf("1. スケジュールの追加\n");
        printf("2. スケジュールの表示\n");
        printf("3. スケジュールの削除\n");
        printf("4. 保存して終了\n");
        printf("選択肢を入力してください: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addSchedule(schedules, &scheduleCount);
            break;
        case 2:
            displaySchedules(schedules, scheduleCount);
            break;
        case 3:
            deleteSchedule(schedules, &scheduleCount);
            break;
        case 4:
            saveSchedules(schedules, scheduleCount, filename);
            printf("スケジュールを保存しました。プログラムを終了します。\n");
            break;
        default:
            printf("無効な選択肢です。再試行してください。\n");
        }
    } while (choice != 4);

    return 0;
}

// ファイルからスケジュールを読み込む
void loadSchedules(Schedule schedules[], int *count, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("既存のスケジュールファイルが見つかりませんでした。新規作成します。\n");
        return;
    }

    while (fscanf(file, "%d %d %d %d %d %[^\n]",
                  &schedules[*count].year,
                  &schedules[*count].month,
                  &schedules[*count].day,
                  &schedules[*count].hour,
                  &schedules[*count].minute,
                  schedules[*count].description) == 6)
    {
        (*count)++;
        if (*count >= MAX_SCHEDULES)
            printf("スケジュールの上限に達しました。。\n");
        break;
    }

    fclose(file);
    printf("スケジュールを読み込みました。\n");
}

// スケジュールをファイルに保存する
void saveSchedules(Schedule schedules[], int count, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("スケジュールをファイルに保存する際にエラーが発生しました。\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%d %d %d %d %d %s\n",
                schedules[i].year,
                schedules[i].month,
                schedules[i].day,
                schedules[i].hour,
                schedules[i].minute,
                schedules[i].description);
    }

    fclose(file);
    printf("スケジュールを保存しました。\n");
}

// 新しいスケジュールを追加する
void addSchedule(Schedule schedules[], int *count)
{
    if (*count >= MAX_SCHEDULES)
    {
        printf("スケジュールのリストが満杯です。これ以上追加できません。\n");
        return;
    }

    Schedule newSchedule;
    printf("年を入力してください: ");
    scanf("%d", &newSchedule.year);
    printf("月を入力してください: ");
    scanf("%d", &newSchedule.month);
    printf("日を入力してください: ");
    scanf("%d", &newSchedule.day);
    printf("時を入力してください: ");
    scanf("%d", &newSchedule.hour);
    printf("分を入力してください: ");
    scanf("%d", &newSchedule.minute);

    if (!isValidDate(newSchedule.year, newSchedule.month, newSchedule.day, newSchedule.hour,
                     newSchedule.minute))
    {
        printf("無効な日時です。もう一度入力してください。\n");
        return;
    }

    getchar(); // 改行文字を消費
    printf("説明を入力してください: ");
    fgets(newSchedule.description, sizeof(newSchedule.description), stdin);

    // 改行が入った場合は取り除く
    for (int i = 0; newSchedule.description[i] != '\0'; i++)
    {
        if (newSchedule.description[i] == '\n')
        {
            newSchedule.description[i] = '\0';
            break;
        }
    }

    newSchedule.dayOfWeek = calculateDayOfWeek(newSchedule.year, newSchedule.month, newSchedule.day);

    schedules[*count] = newSchedule;
    (*count)++;

    sortSchedules(schedules, *count);
    printf("スケジュールを追加しました。\n");
}

// 全てのスケジュールを表示する
void displaySchedules(Schedule schedules[], int count)
{
    if (count == 0)
    {
        printf("表示するスケジュールがありません。\n");
        return;
    }

    printf("\n--- スケジュール一覧 ---\n");
    for (int i = 0; i < count; i++)
    {
        printf("%04d-%02d-%02d (%s) %02d:%02d - %s\n",
               schedules[i].year,
               schedules[i].month,
               schedules[i].day,
               getDayOfWeekString(schedules[i].dayOfWeek),
               schedules[i].hour,
               schedules[i].minute,
               schedules[i].description);
    }
}

// スケジュールを削除する
void deleteSchedule(Schedule schedules[], int *count)
{
    if (*count == 0)
    {
        printf("削除するスケジュールがありません。\n");
        return;
    }

    int index;
    displaySchedules(schedules, *count);
    printf("削除するスケジュールの番号を入力してください (1-%d): ", *count);
    scanf("%d", &index);

    if (index < 1 || index > *count)
    {
        printf("無効な番号です。\n");
        return;
    }

    for (int i = index - 1; i < *count - 1; i++)
    {
        schedules[i] = schedules[i + 1];
    }
    (*count)--;
    printf("スケジュールを削除しました。\n");
}

// スケジュールを比較する（ソート用）
int compareSchedules(const Schedule *a, const Schedule *b)
{
    if (a->year != b->year)
        return a->year - b->year;
    if (a->month != b->month)
        return a->month - b->month;
    if (a->day != b->day)
        return a->day - b->day;
    if (a->hour != b->hour)
        return a->hour - b->hour;
    return a->minute - b->minute;
}

// バブルソートでスケジュールをソートする
void sortSchedules(Schedule schedules[], int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (compareSchedules(&schedules[j], &schedules[j + 1]) > 0)
            {
                Schedule temp = schedules[j];
                schedules[j] = schedules[j + 1];
                schedules[j + 1] = temp;
            }
        }
    }
}

// 有効な日時か判定する
int isValidDate(int year, int month, int day, int hour, int minute)
{
    if (month < 1 || month > 12)
        return 0; // 月が無効
    if (day < 1)
        return 0; // 日が無効
    if (hour < 0 || hour > 23)
        return 0;
    if (minute < 0 || minute > 59)
        return 0;

    // 各月の日数
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // うるう年の場合2月の29日を考慮
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        daysInMonth[1] = 29;
    }

    return day <= daysInMonth[month - 1]; // 日がその月の日数以内か確認
}

DayOfWeek calculateDayOfWeek(int year, int month, int day)
{
    if (month < 3)
    {
        year--;
        month += 12;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + k + (k / 4) + (j / 4) - (2 * j)) % 7;
    return (DayOfWeek)((h + 5) % 7); // 0: SUNDAY, ..., 6: SATURDAY
}

const char *getDayOfWeekString(DayOfWeek day)
{
    const char *days[] = {"日", "月", "火", "水", "木", "金", "土"};
    return days[day];
}