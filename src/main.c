#include<dirent.h>
#include<errno.h> //strerror()
#include<ftw.h> //ntfw()
#include<pwd.h> // get_pwd_uid()
#include<stdio.h>
#include<string.h>
#include<sys/stat.h> // mkdir()
#include<sys/types.h> //mkdir()
#include<unistd.h> // getuid()

#define TRUE 0
#define FALSE 1
#define APP_NAME "cklist"
#define APP_DESCRIPTION "The simple checklist manager"
#define APP_ROOT_DIR ".simulation-checklist/"
#define VERSION "1.0"

//TODO - int check_if_directory_exists(char *);
//TODO - int check_if_file_exists(char *);
int create_checklist(char *);
int create_checklist_item(char *, char *);
int create_app_directory();
void delete_checklist(char *);
void delete_checklist_item(char *, char *);
char *get_app_directory();
char *get_home_directory();
void list_checklists();
void list_checklist_items(char *);
void list_directory_contents(char *, char *);
void print_help();
void print_version_info();

int main(int argsCount, char **args)
{
	printf("\n");

	if (argsCount <= 1) {
		print_help();
		return 0;
	}

	for (int i = 1; i < argsCount; i++) {
		char *arg = args[i];

		if (strcmp(arg, "--help") == TRUE || strcmp(arg, "-h") == TRUE)
			print_help();
		if ((strcmp(arg, "--create") == TRUE || strcmp(arg, "-c") == TRUE) && args[i+1] != NULL)
			create_checklist(args[i+1]);

		if ((strcmp(arg, "--add") == TRUE || strcmp(arg, "-a") == TRUE)) {
			if (args[i+1] == NULL) {
				printf("Error: Checklist name required. Exiting\n");
				break;
			}
			if (args[i+2] == NULL) {
				printf("Error: Item name required. Exiting\n");
				break;
			}
			create_checklist_item(args[i+1], args[i+2]);
		}

		if ((strcmp(arg, "--delete") == TRUE || strcmp(arg, "-d") == TRUE)) {
			if (args[i+1] == NULL) {
				printf("Error: Checklist name required. Exiting\n");
				break;
			}
			if (args[i+2] == NULL) {
				delete_checklist(args[i+1]);
				break;
			}
			delete_checklist_item(args[i+1], args[i+2]);
		}

		if (strcmp(arg, "--list") == TRUE || strcmp(arg, "-l") == TRUE) {
			if (args[i+1] != NULL) {
				list_checklist_items(args[i+1]);
				continue;
			}
			list_checklists();
		}

		if (strcmp(arg, "--init") == TRUE)
			create_app_directory();
		if (strcmp(arg, "-t") == TRUE)
			get_app_directory();
		if (strcmp(arg, "--version") == TRUE || strcmp(arg, "-v") == TRUE)
			print_version_info();
	}

	printf("\n");

	return 0;
}

//TODO - int check_if_directory_exists(char *dirName)
//TODO - int check_if_file_exists(char *);

int create_app_directory()
{
	int errorCode = 0;
	char appDirectory[200] = "";
	char *appDir = get_app_directory();

	strcat(appDirectory, appDir);

	//TODO
	//errorCode = check_if_directory_exists(listDirectory);
	//if (errorCode == -2) return errorCode;

	printf("Creating App Directory...  ");

	// TODO: Fix this with umask() to uid then reset to pid?
	errorCode = mkdir (appDirectory, 0777);

	if (errorCode) {
		printf("\nError: Can't make %s directory:  %s\nExiting...\n", appDir, strerror(errorCode));
		return errorCode;
	}

	printf("Done\n");

	return 0;
}

int create_checklist(char *listName)
{
	if (listName == NULL)
		return -1;

	//DIR *listFolder = NULL;
	int errorCode = 0;

	char listDirectory[200] = "";
	char *appDir = get_app_directory();
	strcat(listDirectory, appDir);
	strcat(listDirectory, listName);

	//TODO
	create_app_directory();
	//errorCode = check_if_directory_exists(listDirectory);
	//if (errorCode == -2) return errorCode;

	printf("Creating checklist %s...  ", listName);
	// TODO: Fix this with umask() to uid then reset to pid?
	errorCode = mkdir(listDirectory, 0777);

	if (errorCode) {
		printf("\nError: Can't make \"%s\" checklist. Exiting...\n", appDir);
		return errorCode;
	}

	printf("Done\n");

	return 0;
}

int create_checklist_item(char *listName, char *itemName)
{
	char itemPath[200] = "";
	strcat(itemPath, get_app_directory());
	strcat(itemPath, listName);
	strcat(itemPath, "/");
	strcat(itemPath, itemName);

	FILE *itemFile;
	itemFile = fopen(itemPath, "w");
	fclose(itemFile);

	printf("\"%s\" added to the \"%s\" checklist\n", itemName, listName);
	return 0;
}

void delete_checklist(char *listName)
{
	printf("Deleting checklist: %s...  ", listName);

	char listPath[200] = "";
	strcat(listPath, get_app_directory());
	strcat(listPath, listName);
	strcat(listPath, "/");

	int status = 0;
	DIR *targetDir;
	struct dirent *entry;
	targetDir = opendir(listPath);

	if (targetDir == NULL) {
		printf("Error: Could not remove the \"%s\" checklist. Exiting\n", listName);
		return;
	}

	while((entry = readdir(targetDir)) != NULL) {
		if (strcmp(entry->d_name, ".") == TRUE || strcmp(entry->d_name, "..") == TRUE)
			continue;

		char currFilePath[200] = "";
		strcpy(currFilePath, listPath);
		strcat(currFilePath, entry->d_name);
		remove(currFilePath);
	}

	(void) closedir(targetDir);
	remove(listPath);

	printf("Done\n");
}

void delete_checklist_item(char *listName, char *itemName)
{
	printf("Deleting %s from %s...  ", itemName, listName);

	char listPath[200] = "";
	strcat(listPath, get_app_directory());
	strcat(listPath, listName);
	strcat(listPath, "/");
	strcat(listPath, itemName);

	int status = 0;
	status = remove(listPath);

	if (status != 0) {
		printf("Error: Could not remove item \"%s\" from the \"%s\" checklist. Exiting\n", itemName, listName);
		return;
	}

	//printf("\"%s\" successfully removed from the \"%s\" checklist\n", itemName, listName);
	printf("Done\n");
}

char *get_app_directory()
{
	char appDir[200] = "";
	strcat(appDir, get_home_directory());
	strcat(appDir, APP_ROOT_DIR);

	char *appDirectory = appDir;
	return appDirectory;
}

char *get_home_directory()
{
	struct passwd *pw = getpwuid(getuid());
	char *homeDir = strcat(pw->pw_dir, "/");
	return homeDir;
}

void list_checklists()
{
	char dirPath[200] = "";
	char *appDirectoryPath = get_app_directory();
	strcat(dirPath, appDirectoryPath);

	list_directory_contents(dirPath, "  > ");
}

void list_checklist_items(char *targetDirectory)
{
	if (targetDirectory == NULL)
		return;

	char dirPath[200] = "";
	char *appDirectoryPath = get_app_directory();
	strcat(dirPath, appDirectoryPath);
	strcat(dirPath, targetDirectory);

	printf(" %s:\n", targetDirectory);
	list_directory_contents(dirPath, "  - ");
}

void list_directory_contents(char *targetDirectoryPath, char *lineDelimiter)
{
	if (lineDelimiter == NULL)
		lineDelimiter = "";

	char directoryPath[1024] = "";
	strcat(directoryPath, targetDirectoryPath);
	strcat(directoryPath, "/");

	DIR *targetDir;
	struct dirent *entry;
	targetDir = opendir(directoryPath);

	if (targetDir != NULL) {
		while (entry = readdir(targetDir)) {
			if (strcmp(entry->d_name, ".") == TRUE || strcmp(entry->d_name, "..") == TRUE)
				continue;
			printf("%s%s\n", lineDelimiter, entry->d_name);
		}
		(void) closedir(targetDir);
		return;
	}

	printf("Error: Could not find or open the checklist under \"%s\". Exiting\n", directoryPath);
}

void print_help()
{
	char *helpMessage = APP_NAME
				": "
				APP_DESCRIPTION
				" v"
				VERSION
				"\nUsage:\n"
				"  simlist -<flags>\n"
				"  simlist -o | --option [<argument>...]\n"
				"\nOptions:\n"
				"  -h | --help                              Display this help message\n"
				"  -c | --create <listname>                 Create a checklist as <listname>\n"
				"  -v | --version                           Show the version info\n"
				"  -l | --list                              Show all checklists\n"
				"  -l | --list <listname>                   Show all items in checklist <listname>\n"
				"  -a | --add <listname> <itemname>         Add <itemname> as a checklist item\n"
				"  -d | --delete <listname> [<itemname>]    Delete the <listname> or the <itemname> from the checklist\n"
				"\nJoshua Spann, 2018\n";
	printf("%s\n", helpMessage);
}

void print_version_info()
{
	printf("simlist v%s\nBuild Nov 2018\n", VERSION);
}
