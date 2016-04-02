#include "parse.h"

/*********************************************************/
/* @brief Parse a .f4m file to obtain bitrates.          */
/* @param state - state of the client to store bitrates. */
/* @param f4m   - The .f4m file.                         */
/*********************************************************/
void parse_f4m(fsm* state, FILE* f4m)
{
  char *buf = NULL, *needle1 = NULL, *needle2 = NULL;
  char[200] number = {0};
  size_t n  = 0, len = 0;
  int bitrate_f4m = 0;
  struct bitrate* rate = NULL;

  /* Getline allocates a null-terminated buffer for you internally. */
  while((getline(&buf, &n, f4m) != -1))
    {
      needle1 = strstr(buf, "bitrate=");

      if(!needle1)
        continue;

      needle2 = strstr(needle1 + strlen("bitrate=") + 1, "\"");
      len     = needle2 - (needle1 + strlen("bitrate=") + 1);

      bzero(number, 200);
      strncpy(number, needle1 + strlen("bitrate=") + 1, len);

      bitrate_f4m = atoi(number);

      rate          = calloc(sizeof(struct bitrate), 1);
      rate->bitrate = bitrate_f4m;
      HASH_ADD_INT(state->all_bitrates, bitrate, rate);
    }

  free(buf);
}

/* Returns a substring of the given string from [start,end). */
void getSubstring(char *dest, char *src, int start, int end){
    strncpy(dest, src + start, end - start);
    strncat(dest, "\0", 1);
}

//Should calculate the bitrate by first finding the throughput and then
//comparing the result to the approprtiate bitrate in the global array.
int calculate_bitrate(struct timespec *start, struct timespec *end, int size){
	return 0;
}

/*
	1. Break the message up into headers and body.
	2. Get the content length header.
	3. If not, just calculate that body length manually.
	4. Calculate the bitrate, set it and return.
*/
void parse_server_message(char *msg){
	return;
}

// Generate the client's message. Should be easy with the struct.
void gen_client_message(client_req *req){
	return;
}

/* Copies some relevant information into my superior struct. */
void copy_info(client_req *my_req, struct state *client){
  memcpy(my_req->req_type, client->method, strlen(client->method));
  memcpy(my_req->URI, client->uri, strlen(client->uri));
  memcpy(my_req->version, client->version, strlen(client->version));
  my_req->bitrate = client->bitrate; //Change later
}

void parse_URI(client_req *my_req){
  char *last_slash = strstr(my_req->URI, "/");
  char temp[BUF_SHORT];
  memset(temp, 0, BUF_SHORT);
  while(!last_slash){
    memcpy(temp, last_slash, strlen(last_slash));
    last_slash = strstr(last_slash, "/");
  }
  memcpy(my_req->file, temp + 1, strlen(temp) - 1);
}

//Assumes we're dealing with video chunk
void parse_file(client_req *my_req){
  int birate;
  int seq_num;
  int frag_num;
  char str_bitrate[BUF_SHORT];
  char str_seq_num[BUF_SHORT];
  char str_frag_num[BUF_SHORT];

  int start_pos = 0;
  int end_pos = 0;

  //First, the bitrate. Useless.
  while(isdigit((my_req->file)[end_pos])){
    end_pos++;
  }
  getSubstring(str_bitrate, my_req->file, start_pos, end_pos);

  //Skip the "seg" string
  while(!isdigit((my_req->file)[end_pos])){
    end_pos++;
  }
  start_pos = end_pos;

  //Get the "seq" number
  while(isdigit((my_req->file)[end_pos])){
    end_pos++;
  }
  getSubstring(str_seq_num, my_req->file, start_pos, end_pos);

  //Skip the "frag" string
  while(!isdigit((my_req->file)[end_pos])){
    end_pos++;
  }
  start_pos = end_pos;

  //Get the "frag" number
  while(isdigit((my_req->file)[end_pos])){
    end_pos++;
  }
  getSubstring(str_frag_num, my_req->file, start_pos, end_pos);

  my_req->segno = atoi(str_seq_num);
  my_req->fragno = atoi(str_frag_num);
}

/*
	1. Get the URI, request type, version, bitrate, blah.
	2. If possible, get the segno. Set the content_type value.
	3. Generate the server response.
*/
void parse_client_message(struct state *client){
  client_req *my_req = calloc(1, client_req *);
  copy_info(my_req, client);
  char *fragment = strstr(client->uri, ".f4f");
  char *manifest = strstr(client->uri, ".f4m");
  if(manifest){
    my_req->content_type = 1;
    my_req->segno = -1;
    my_req->fragno = -1;
  } else if(fragment){
    my_req->content_type = 2;
    parse_URI(my_req);
    parse_file(my_req);
  } else if(!manifest && !fragment){
    my_req->content_type = 0;
    my_req->segno = -1;
    my_req->fragno = -1;
  }
  //Generate response here
}
