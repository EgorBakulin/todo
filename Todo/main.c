#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "todo.h"

bool is_option(char *arg)
{
	return strlen(arg) == 2 && arg[0] == '-';
}

char get_option(char *arg)
{
	if (is_option(arg))
		switch (arg[1]) {
		case 'e':
		case 'm':
		case 'r':
			return arg[1];
		}
	return 0;
}

int get_task_no(char *arg)
{
	return isdigit(arg[0]) ? atoi(arg) : -1;
}

int main(int argc, char *argv[])
{
	/* Create todo list */
	Todo list;
	size_t size = todo_init(&list, "todo.md");

	/* Load existing tasks */
	void *tasks = malloc(size);
	get_tasks(&list, tasks);

	/* Parse and process arguments */
	bool error = false;

	int task_no;

	bool is_add = argc > 1 && !is_option(argv[1]);
	bool is_edit = argc > 3 && get_option(argv[1]) == 'e';

	if (argc == 2 && !is_option(argv[1]) &&
		(task_no = get_task_no(argv[1])) != -1)
		mark_task(&list, task_no);
	else if (argc == 3 && get_option(argv[1]) == 'r')
		remove_task(&list, atoi(argv[2]));
	else if (argc == 4 && get_option(argv[1]) == 'm')
		move_task(&list, atoi(argv[2]), atoi(argv[3]));
	else if (is_add || is_edit) {
		/* Concatenate arguments when adding or editing a task */
		task_no = get_task_no(argv[1]);
		int start = is_edit ? 3 : task_no != -1 ? 2 : 1;

		int total_len = 0;
		for (int i = start; i < argc; i++)
			total_len += strlen(argv[i]) + 1;

		char *task = malloc(total_len);
		task[0] = '\0';
		for (int i = start; i < argc; i++) {
#ifdef _MSC_VER
			strcat_s(task, total_len, argv[i]);
#else
			strcat(task, argv[i]);
#endif
			if (i < argc - 1)
#ifdef _MSC_VER
				strcat_s(task, total_len, " ");
#else
				strcat(task, " ");
#endif
		}

		if (is_edit)
			edit_task(&list, atoi(argv[2]), task);
		else if ((size = add_task(&list, task)) && task_no != -1) {
			tasks = realloc(tasks, size);
			get_tasks(&list, tasks);
			move_task(&list, list.length, task_no);
		}
	}
	else if (argc > 1)
		error = true;

	/* Print tasks/help */
	if (error)
		printf(
			"usage:\n\t"
			"show: todo\n\t"
			"add: todo buy milk\n\t"
			"     todo \"eat cookies\"\n\t"
			"edit: todo -e 1 buy more milk\n\t"
			"move: todo -m 2 1\n\t"
			"mark: todo 1\n\t"
			"remove: todo -r 1\n"
		);
	else
		print_tasks(&list);

	/* Cleanup */
	free(tasks);
	fclose(list.file);

    return 0;
}
