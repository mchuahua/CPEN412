
void MemoryTest(void)
{
    unsigned long long int *RamPtr;
    unsigned int counter1=0x8cb;
    register unsigned int i ;
    unsigned int Start, End ;
    char c ;
    unsigned long int data, write_data;
    unsigned int bwl=0;

    // input validation of addresses, user should know that for word and long word addresses shoudl align to even.
    do{
        printf("\r\nStart Address (min 0x08020000 max 0x08030000): ") ;
        Start = Get8HexDigits(0) ;
    } while (0x08020000 > Start || 0x08030000 < Start);
    // Start = 0x08020000;
    do {
        printf("\r\nEnd Address (min 0x08020000 max 0x08030000): ") ;
        End = Get8HexDigits(0) ;
    } while (Start > End || End > 0x08030000);
    // End = 0x08030000;

	// Get test pattern and set data appropriately
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

    // Let user know what they selected
    printf("\r\nData selected: 0x%x", data);

    // Get size of test pattern and set data size appropriately
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
	
    // Let user know what they selected
    printf("\r\nbwl selected:");
    if(bwl == (unsigned int)(0x3)) printf("Long word");
    if(bwl == (unsigned int)(0x2)) printf("Word");
    if(bwl == (unsigned int)(0x1)) printf("Bytes");

    // Set pointer to start addr
    RamPtr = Start;
    // Write loop
    while(1){
        // When end addr is reached
        if (RamPtr > End){
            printf("\r\nWrite complete. Starting read.");
            break;
        }
        // Set data at address to specified by user
        *RamPtr = data;
        
        counter1++;
        // Dont check every time, just check some time incl first time
        if (counter1 == 0x8cc){
            printf("\r\nWrite: 0x%x to addr 0x%x", *RamPtr, RamPtr);
            counter1 = 1;
        }
        // Increment address
        RamPtr++;
    }
    
    // Reset pointer to start addr
    RamPtr = Start;
    // Reset counter to default
    counter1 = 0x8cb;

    // Read loop
    while(1){
        // When end addr is reached
        if (RamPtr > End){
            printf("\r\nRead complete.");
            printf("\r\nPASS: Mem test completed with no errors.");
            break;
        }
        // Read check every address to specified data by user
        if (*RamPtr != data){
            printf("\r\nERROR: Address 0x%x data is 0x%x but should be 0x%x", RamPtr, *RamPtr, data);
            printf("\r\nFAIL: Mem test did not complete successfully.");
            break;
        }
        counter1++;
        // Dont check every time, just check some time incl first time
        if (counter1 == 0x8cc){
            printf("\r\nRead: Address 0x%x data is 0x%x", RamPtr, *RamPtr);
            counter1 = 1;
        }
        RamPtr++;
    }
}   
