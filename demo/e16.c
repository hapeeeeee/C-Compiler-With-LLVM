// int printf(const char *fmg, ...);

int main ()
{
   /* 局部变量定义 */
   // char grade = 'E';
   int grade = 0;
   switch(grade)
   {
   case 0 : {
      break;
   }
   case 1 : 
   grade = 100;
      // printf("ddd\n");
      // break;
   case 2 : 
   grade = 200;

      break;
   case 3 : {
      grade = 300;
      break;
   }

   default : {
      grade = 1000;
      break;
   }
   }

 
   return grade;
}
