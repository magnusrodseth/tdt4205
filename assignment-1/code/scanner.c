#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_STATES 9
#define START_STATE 0
#define ACCEPT 8
#define ERROR 9
#define ALPHABET_SIZE 256

int transition_table[NUMBER_OF_STATES]
                    [ALPHABET_SIZE]; // Table form of the automaton

void initialize_transition_table()
{
  for (int row = 0; row < NUMBER_OF_STATES; row++)
  {
    for (int col = 0; col < ALPHABET_SIZE; col++)
    {
      transition_table[row][col] = ERROR;
    }
  }

  transition_table[0]['d'] = 1;
  transition_table[0]['g'] = 6;
  transition_table[1]['x'] = 2;
  transition_table[1]['y'] = 2;
  transition_table[2]['='] = 3;
  transition_table[3]['-'] = 4;
  for (int i = '0'; i <= '9'; i++)
  {
    transition_table[3][i] = 5;
  }
  for (int i = '0'; i <= '9'; i++)
  {
    transition_table[4][i] = 5;
  }
  for (int i = '0'; i <= '9'; i++)
  {
    transition_table[5][i] = 5;
  }
  transition_table[5]['\n'] = ACCEPT;
  transition_table[6]['o'] = 7;
  transition_table[7]['\n'] = ACCEPT;
}

// Driver program's internal state
int state = START_STATE;
int x = 421, y = 298, // We start at the middle of the page,
    dx = 0, dy = 0;   // and with dx=dy=0

// Used to store the chars of statement we are currently reading
char lexeme_buffer[1024];
int lexeme_length = 0;

// In here we can assume that lexeme_buffer contains a valid statement, since
// the DFA reached ACCEPT
void handle_statement()
{
  if (strncmp(lexeme_buffer, "go", 2) == 0)
  {
    x = x + dx;
    y = y + dy;
    printf("%d %d lineto\n", x, y);
    printf("%d %d moveto\n", x, y);
  }
  else if (strncmp(lexeme_buffer, "dx=", 3) == 0)
  {
    sscanf(lexeme_buffer + 3, "%d", &dx);
  }
  else if (strncmp(lexeme_buffer, "dy=", 3) == 0)
  {
    sscanf(lexeme_buffer + 3, "%d", &dy);
  }
  else
  {
    assert(0 && "Reached an unreachable branch!");
  }
}

int main()
{
  // Setup the DFA transitions as a table
  initialize_transition_table();

  // PostScript preable to create a valid ps-file
  printf("<< /PageSize [842 595] >> setpagedevice\n");
  printf("%d %d moveto\n", x, y);

  // Main loop
  int line_num = 1; // Used to report which line an error occured on
  int read;
  while ((read = getchar()) != EOF)
  {
    // Store the read char in the buffer
    lexeme_buffer[lexeme_length++] = read;
    lexeme_buffer[lexeme_length] = 0; // Add NULL terminator

    // Use the current state and the read char to find the next state
    state = transition_table[state][read];

    // Check if we reached the ACCEPT or ERROR states
    switch (state)
    {
    case ACCEPT:
      handle_statement();
      state = START_STATE;
      lexeme_length = 0;
      break;
    case ERROR:
      fprintf(stderr, "error: %d: unrecognized statement: %s\n", line_num,
              lexeme_buffer);
      exit(EXIT_FAILURE);
    default:
      break;
    }

    // If the char was a newline, the next char will be on a new line!
    if (read == '\n')
      line_num++;
  }

  if (state != START_STATE)
  {
    fprintf(stderr, "error: %d: input ended in the middle of a statement: %s\n",
            line_num, lexeme_buffer);
    exit(EXIT_FAILURE);
  }

  printf("stroke\n");
  printf("showpage\n");
}
