float a;
int b;
char c;
void main()
{
    send_int_to_robot(1,{1,2});
    send_int_to_robot(1,b);
    send_float_to_robot(2,{3.0,4.0});
    send_float_to_robot(2,3.14);
    send_string_to_robot(3,"abc");
    send_string_to_robot(3,c);
    b = receive_int_from_robot(b);
    a = receive_float_from_robot(b);
    {/*
    char rc[] = receive_string_from_robot(b);
        ;
    */}
    return;
}
   
