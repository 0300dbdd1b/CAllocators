#if 0
cc CBuilder.c -o CBuilder
./CBuilder
rm ./CBuilder
exit
#endif

#ifndef		_CBUILD_
	#define	_CBUILD_
	#define	BUILDER_SOURCE		"CBuilder.c"
	#define	BUILDER_BASE_NAME	"CBuilder"
	#include <time.h>

	#if __has_include("platform.h")
		#include "platform.h"
	#else
		#include <stdint.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <string.h>
		#include <unistd.h>
		#include <stdarg.h>
		typedef uint8_t U8;
	#endif

	#if defined(_WIN32) || defined(_WIN64)
		#ifndef		PLATFORM_WINDOWS
			#define	PLATFORM_WINDOWS
		#endif
		#define BUILDER_NAME	BUILDER_BASE_NAME ".exe"
		#include <windows.h>
	#elif defined(__linux__) || defined(__APPLE__) && defined(__MACH__)
		#ifndef		PLATFORM_UNIX
			#define	PLATFORM_UNIX
		#endif
		#define BUILDER_NAME	"./" BUILDER_BASE_NAME
		#include <sys/types.h>
		#include <sys/wait.h>
		#include <sys/stat.h>
		#include <unistd.h>
		#include <pthread.h>
		#include <stdarg.h>

		#if defined(__linux__)
			#ifndef		PLATFORM_LINUX
				#define	PLATFORM_LINUX
			#endif
		#elif defined(__APPLE__) && defined(__MACH__)
			#ifndef		PLATFORM_MACOS
				#define	PLATFORM_MACOS
			#endif
		#else
			#error "Unsupported platform"
		#endif
	#endif

#define		DEFAULT_STRING_ALLOCATION_SIZE	500
#define		MAX_RULES						100
#define		MAX_COMMAND						100

#ifndef			SOURCE_LAST_EDIT
	#define		SOURCE_LAST_EDIT				(time_t)0
#endif

typedef struct String
{
	char *c_str;              // Pointer to the character array
	unsigned int length;      // Current length of the string
	unsigned int size;        // Allocated size of the string buffer
} String;

typedef String Command;

String *CreateString(void)
{
	String *string = (String *)malloc(sizeof(String));
	string->c_str = (char *)malloc(DEFAULT_STRING_ALLOCATION_SIZE * sizeof(char));
	string->length = 0;
	string->size = DEFAULT_STRING_ALLOCATION_SIZE;
	string->c_str[0] = '\0';

	return string;
}
#define CreateCommand CreateString

void AppendToString(String *str, const char *element)
{
	if (!str || !element) return;

	unsigned long elementLength = strlen(element);
	unsigned long requiredSize = str->length + elementLength + 1;

	if (requiredSize > str->size)
	{
		unsigned int newSize = str->size * 2;
		while (newSize < requiredSize)
		{
			newSize *= 2; 
		}
		char *newBuffer = (char *)realloc(str->c_str, newSize);
		str->c_str = newBuffer;
		str->size = newSize;
	}

	memcpy(str->c_str + str->length, element, elementLength);
	str->length += elementLength;
	str->c_str[str->length] = '\0';
}


void AppendToStringNULL(String *str, ...)
{
    if (!str) return;

    va_list args;
    va_start(args, str);

    const char *arg = va_arg(args, const char *);
    while (arg != NULL)
    {
        if (str->length > 0)
        {
            AppendToString(str, " ");
        }

        AppendToString(str, arg);
        arg = va_arg(args, const char *);
    }
    va_end(args);
}
#define AppendToCommandNULL AppendToStringNULL
#define AppendToCommand(command, ...) AppendToCommandNULL(command, __VA_ARGS__, NULL)

typedef void(*RuleFunction)(void);
typedef struct Rule
{
	char			*name;
	RuleFunction	function;
} Rule;

Rule gRules[MAX_RULES];
int	gRuleCount;


void CreateRule(const char *ruleName, RuleFunction function) {
	if (gRuleCount >= MAX_RULES)
		return;
	gRules[gRuleCount].name = strdup(ruleName);
	gRules[gRuleCount].function = function;
	gRuleCount++;
}

U8 ExecuteRule(const char *rule)
{
	for (int i = 0; i < gRuleCount; i++)
	{
		if (strcmp(gRules[i].name, rule) == 0)
		{
			printf("Executing rule %s\n", gRules[i].name);
			gRules[i].function();
			return 1; } } return 0;
}


int ExecuteCommandGeneric(const Command *command, int runInThread) {
	if (!command || !command->c_str) {
		fprintf(stderr, "Invalid command: NULL pointer\n");
		return -1;
	}

	printf("[CMD] : %s\n", command->c_str);

#ifdef PLATFORM_WINDOWS
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char *cmdBuffer = strdup(command->c_str);
	if (!cmdBuffer) {
		fprintf(stderr, "Memory allocation failed for command buffer\n");
		return -1;
	}

	if (!CreateProcess(NULL, cmdBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		fprintf(stderr, "Command execution failed: %lu\n", GetLastError());
		free(cmdBuffer);
		return -1;
	}

	if (runInThread) {
		HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WaitForSingleObject, pi.hProcess, 0, NULL);
		if (thread == NULL) {
			fprintf(stderr, "Failed to create thread: %lu\n", GetLastError());
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			free(cmdBuffer);
			return -1;
		}
		WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
	} else {
		WaitForSingleObject(pi.hProcess, INFINITE);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	free(cmdBuffer);
#elif defined(PLATFORM_UNIX)
	pid_t pid = fork();
	if (pid < 0) {
		perror("Fork failed");
		return -1;
	}

	if (pid == 0) {
		execl("/bin/sh", "sh", "-c", command->c_str, (char *)NULL);
		perror("execl failed");
		exit(EXIT_FAILURE);
	} else {
		if (runInThread) {
			pthread_t waitThread;
			pthread_create(&waitThread, NULL, (void *(*)(void *))waitpid, (void *)&pid);
			pthread_join(waitThread, NULL);
		} else {
			int status;
			waitpid(pid, &status, 0);
			return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
		}
	}
#endif

	return 0;
}

#define ExecuteCommand(command) ExecuteCommandGeneric(command, 0)
#define ExecuteCommandWithThreading(command) ExecuteCommandGeneric(command, 1)

void ExecuteCommands(int runInThread, ...)
{
	va_list args;
	va_start(args, runInThread);

	pthread_t *threads = NULL;
	int threadCount = 0;
	if (runInThread)
		threads = (pthread_t *)malloc(sizeof(pthread_t) * MAX_COMMAND); // Arbitrary maximum command count

	Command *cmd;
	while ((cmd = va_arg(args, Command *)) != NULL)
	{
		if (runInThread)
		{
			pthread_create(&threads[threadCount], NULL, (void *(*)(void *))ExecuteCommandGeneric, (void *)cmd);
			threadCount++;
		}
		else
		ExecuteCommand(cmd);
	}

	if (runInThread) {
		for (int i = 0; i < threadCount; i++) {
			pthread_join(threads[i], NULL);
		}
		free(threads);
	}

	va_end(args);
}
#define ExecuteCommands(...) ExecuteCommands(__VA_ARGS__, NULL)

time_t GetFileLastEditTime(const char *filePath)
{
    struct stat fileStat;
    if (stat(filePath, &fileStat) != 0)
    {
        perror("Failed to get file stats");
        return 0;
    }
    return fileStat.st_mtime;
}

void RebuildSelf(void)
{
    Command *selfBuildCmd = CreateCommand();
#ifdef PLATFORM_WINDOWS
    AppendToCommand(selfBuildCmd, "gcc", BUILDER_SOURCE, "-o", BUILDER_NAME);
#else
    AppendToCommand(selfBuildCmd, "gcc", BUILDER_SOURCE, "-o", BUILDER_NAME);
#endif
    printf("Rebuilding CBuilder: %s\n", selfBuildCmd->c_str);

    int result = ExecuteCommand(selfBuildCmd);
    if (result != 0)
    {
        fprintf(stderr, "Failed to rebuild CBuilder with code: %d\n", result);
        exit(EXIT_FAILURE);
    }

    printf("CBuilder rebuilt successfully.\n");
    free(selfBuildCmd->c_str);
    free(selfBuildCmd);
}

void RunSelf(String *args)
{
    Command *cmd = CreateCommand();
#ifdef PLATFORM_WINDOWS
    AppendToCommand(cmd, BUILDER_NAME);
#else
    AppendToCommand(cmd, "./" BUILDER_NAME);
#endif
    AppendToCommand(cmd, args->c_str);
    printf("CMD %s\n", cmd->c_str);
    ExecuteCommand(cmd);
}

void ManageRebuild(int argc, char **argv)
{
    time_t sourceEditTime = GetFileLastEditTime(BUILDER_SOURCE);
    time_t execEditTime = GetFileLastEditTime(BUILDER_NAME);

    if (sourceEditTime > execEditTime)
    {
        printf("Source file is newer than executable. Rebuilding...\n");
        String *args = CreateString();
        for (int i = 1; i < argc; i++)
        {
            AppendToCommand(args, argv[i]);
        }
        RebuildSelf();
        RunSelf(args);
        exit(0);
    }
}
void ManageRules(int argc, char **argv)
{
	U8 retValue;
	ManageRebuild(argc, argv);
	for (int i = 1; i < argc; i++)
		retValue = ExecuteRule(argv[i]);
	if (!retValue && gRuleCount)
		gRules[0].function();
}




/* ----------------------------------------- BUILD MAIN ----------------------------------------- */

#if defined(PLATFORM_UNIX)
	#define EXECNAME	"./test"
	#define CC			"gcc"
	#define CFLAGS		"-Wall -Wextra"
	#define LDFLAGS		""
#else
	#define EXECNAME	"test.exe"
	#define CC			"msvc"
#endif

#ifndef EXECNAME
	#define EXECNAME " "
#endif
#ifndef CC
	#define CC " "
#endif

void DebugRule(void)
{
	Command *cmd = CreateCommand();
	AppendToCommand(cmd, CC, "-g3 -fsanitize=address");
	AppendToCommand(cmd, "main_test.c", "CAllocators.c");
	AppendToCommand(cmd, "-o", EXECNAME);
	ExecuteCommand(cmd);
}

void BuildRule(void)
{
	Command *cmd = CreateCommand();
	AppendToCommand(cmd, CC, "-Wall -Wextra");
	AppendToCommand(cmd, "main.c");
	AppendToCommand(cmd, "-o", EXECNAME);
	ExecuteCommand(cmd);
}

void ExecRule(void)
{
	BuildRule();
	Command *execCommand = CreateCommand();
	AppendToCommand(execCommand, EXECNAME);
	ExecuteCommand(execCommand);
}

void ExecDebug(void)
{
	DebugRule();
	Command *execCommand = CreateCommand();
	AppendToCommand(execCommand, "valgrind", EXECNAME);
	ExecuteCommand(execCommand);
}

int main(int argc, char **argv)
{
	CreateRule("debug", DebugRule);
	CreateRule("run debug", ExecDebug);
	CreateRule("build", BuildRule);
	CreateRule("exec", ExecRule);

	ManageRules(argc, argv);
    return 0;
}



#endif
