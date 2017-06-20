#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void shaderSourcePos(const char *text,int *verPos,int *extPos,int *bodyPos,int *headLines) {
  int textLen,pos,i;

  //
  textLen=strlen(text);
  pos=0;
  *verPos=-1;
  *extPos=-1;
  *bodyPos=-1;

  //
  while(pos < textLen) {
    if(text[pos]==' ' || text[pos]=='\t' || text[pos]=='\r') {
      //skip white space
      pos++;
    } else if(text[pos]=='\n') { //skip newlines
      pos++;
    } else if(text[pos]=='/' && pos+1 < textLen && text[pos+1]=='/') {
      //skip single line comments
      pos+=2;

      while(pos < textLen && text[pos]!='\n') {
        pos++;
      }

      pos++;
    } else if(text[pos]=='/' && pos+1 < textLen-1 && text[pos+1]=='*') {
      //skip double lines comments
      pos+=2;

      while(pos+1< textLen && text[pos]!='*' && text[pos+1]!='/') {
        pos++;
      }
    } else if(text[pos]=='#') { //
      int hashStart=pos;
      pos++;

      //whitespace
      while(text[pos]==' ' || text[pos]=='\t') {
        pos++;
      }

      if(pos+7<textLen && text[pos]=='v' && text[pos+1]=='e'
         && text[pos+2]=='r' && text[pos+3]=='s' && text[pos+4]=='i'
         && text[pos+5]=='o' && text[pos+6]=='n') {
        //on version

        pos+=7;
        while(text[pos++]!='\n'); //pos=after version line
        *bodyPos=pos;
        *verPos=hashStart;
      } else if(pos+9<textLen && text[pos]=='e' && text[pos+1]=='x' &&
                text[pos+2]=='t' && text[pos+3]=='e' && text[pos+4]=='n' &&
                text[pos+5]=='s' && text[pos+6]=='i' &&
                text[pos+7]=='o' && text[pos+8]=='n') {
        //on extension

        pos+=9;
        while(text[pos++]!='\n'); //pos=after version line
        *bodyPos=pos;

        if(*extPos==-1) {
          *extPos=hashStart;
        }

      } else {
        //unknown directive, skip
        pos++;
      }
    } else {
      //version/extension directives not at head of file
      pos++;
    }
  }

  //
  *headLines=0;

  for(i=0;i<(*bodyPos);i++) {
    if(text[i]=='\n') {
      (*headLines)++;
    }
  }
}

bool stringFromFile(const char *fn, char **str) {
  FILE *file;
  unsigned int dataSize;

  //open file
  file = fopen(fn, "rb");

  if(!file) {
    return false;
  }

  //get data size
  fseek(file,0L,SEEK_END);
  dataSize = ftell(file);
  fseek(file,0L,SEEK_SET);

  //
  (*str)=(char*)malloc(dataSize+1);

  //read data
  (*str)[dataSize]=0;//is not already null terminated?
  fread((*str),1,dataSize,file);

  //close file
  fclose(file);
  return true;
}

int main() {
  char *str;
  if(!stringFromFile("light.fs",&str)) {
    return 11;
  }
  int verPos,extPos,bodyPos,headLines;
  shaderSourcePos(str,&verPos,&extPos,&bodyPos,&headLines);


  printf("%i %i : %i : %i\n",verPos,extPos,bodyPos,headLines);
  return 0;
}
