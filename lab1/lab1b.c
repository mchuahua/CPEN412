void MemoryTest(void)
{
    unsigned long long int *RamPtr, counter1=1 ;
    register unsigned int i ;
    unsigned int Start, End ;
    char c ;
    unsigned long int data, write_data;
    unsigned int bwl=0;

    // No input validation of addresses, user should know that for word and long word addresses shoudl align to even.
    printf("\r\nStart Address (min 0x08020000 max 0x08030000): ") ;
    // Start = Get8HexDigits(0) ;
    Start = 0x08020000;
    printf("\r\nEnd Address (min 0x08020000 max 0x08030000): ") ;
    // End = Get8HexDigits(0) ;
    End = 0x08030000;

	// Get test pattern
    while(1)    {
        FlushKeyboard() ;               // dump unread characters from keyboard
        printf("\r\nChoose test pattern: \r\n0: 55\r\n1: AA\r\n2: FF\r\n3: 00");
        printf("\r\n#") ;
        c = toupper(_getch());

        if( c == (char)('0')){                 
            data = 0x55;
            break;
        }
        else if( c == (char)('1')){
            data = 0xAA;
            break;
        }
        else if( c == (char)('2')){
            data = 0xFF;
            break;
        }
        else if( c == (char)('3')){
            data = 0x00;
            break;
        }
    }

    printf("\r\nData selected: 0x%x", data);

    // Get size of test pattern
    while(1)    {
        FlushKeyboard() ;               // dump unread characters from keyboard
        printf("\r\nEnter 'B', 'W', or 'L' for bytes, words, or long word: ");
        printf("\r\n#") ;
        c = toupper(_getch());

        if( c == (char)('B')){
            bwl = 0x1;
            break;
        }
        else if( c == (char)('W')){
            bwl = 0x2;
            data = data | data << 8;
            break;
        }
        else if( c == (char)('L')){
            bwl = 0x3;
            data = data | data << 8 | data << 16 | data << 24;
            break;
        }   
    }
	
    printf("\r\nbwl selected:");
    if(bwl == (unsigned int)(0x3)) printf("Long word");
    if(bwl == (unsigned int)(0x2)) printf("Word");
    if(bwl == (unsigned int)(0x1)) printf("Bytes");

    RamPtr = Start;
    // Write loop
    while(1){
        // When end addr is reached
        if (RamPtr > End){
            printf("\r\nWrite complete. Starting read.");
            break;
        }
        *RamPtr = data;
        
        counter1++;
        if (counter1 == 31){
            printf("\r\nWrite: 0x%x to addr 0x%x", *RamPtr, RamPtr);
            counter1 = 1;
        }
        RamPtr++;
    }
    
    RamPtr = Start;
    counter1 = 1;

    // Read loop
    while(1){
        if (RamPtr > End){
            printf("\r\nPASS: Mem test completed with no errors.");
            break;
        }
        if (*RamPtr != data){
            printf("\r\nERROR: Address 0x%x data is 0x%x but should be 0x%x", RamPtr, *RamPtr, data);
            printf("\r\nStart is 0x%x", Start);
            break;
        }
        counter1++;
        if (counter1 == 31){
            printf("\r\nRead: Address 0x%x data is 0x%x", RamPtr, *RamPtr);
            counter1 = 1;
        }
        RamPtr++;
    }
}   