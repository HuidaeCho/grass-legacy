/* %W% %G% */
input (prompt, answer)
    char *prompt;
    char *answer;
{
    if (isatty(0))
	printf ("%s ", prompt);
    if (!gets(answer)) exit(0);
    G_strip (answer);

    return *answer != 0 ;
}
