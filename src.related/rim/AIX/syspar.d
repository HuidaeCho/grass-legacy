C        -RIM-
C
C        SYSTEM AND INSTALLATION DEPENDENT PARAMETERS
C
c        *** VAX/UNIX edition    (Jun 20, 1989)
c
C        ALL VARIABLES ARE TYPE INTEGER UNLESS OTHERWISE SPECIFIED
C
C
      IMPLICIT INTEGER (A-Z)
C
C     ------------- WORD AND NAME LENGTH -------------
C
C        ZCW       = NUMBER OF CHARACTERS IN A WORD
C        ZC        = NUMBER OF CHARACTERS IN A NAME
C        Z         = NUMBER OF INTEGERS PER NAME
C
      PARAMETER (ZC=16)
      PARAMETER (ZCW=4)
      PARAMETER (Z=(ZC-1)/ZCW+1)
C
C        ZKEYWL       = MAX CHARACTERS IN A KEYWORD
C
      PARAMETER (ZKEYWL=12)
C
C
C     ------------- PHYSICAL RECORD LENGTH -------------
C
C        ZF1       = LENGTH OF FILE 1 PAGE
C        ZF2       = LENGTH OF FILE 2 PAGE
C        ZF3       = LENGTH OF FILE 3 PAGE
C
      PARAMETER (ZF1=1024)
      PARAMETER (ZF2=1024)
      PARAMETER (ZF3=126)
C
C
C     ------------- FILE HEADERS -------------
C
C        ZF1HDR    = LENGTH OF DATABASE FILE 1 HEADER
C        ZF2HDR    = LENGTH OF DATABASE FILE 2 HEADER
C        ZF3HDR    = LENGTH OF DATABASE FILE 3 HEADER
C        ZFXH..    = POINTERS TO ITEMS IN ALL HEADERS
C        ZF1H..    = POINTERS TO ITEMS IN FILE 1
C        ZF2H..    = POINTERS TO ITEMS IN FILE 2
C        ZF3H..    = POINTERS TO ITEMS IN FILE 3
C
C        ZFXHID    = GRIM DATABASE IDENTIFICATION
C        ZFXHVS    = GRIM VERSION NUMBER
C        ZFXHDB    = DATABASE NAME
C        ZFXHOW    = OWNER NAME
C        ZFXHDT    = LAST UPDATE DATE
C        ZFXHTM    = LAST UPDATE TIME
C
      PARAMETER (ZFXHID=1)
      PARAMETER (ZFXHVS=ZFXHID+Z)
      PARAMETER (ZFXHDB=ZFXHVS+1)
      PARAMETER (ZFXHOW=ZFXHDB+Z)
      PARAMETER (ZFXHDT=ZFXHOW+Z)
      PARAMETER (ZFXHTM=ZFXHDT+1)
C
C        ZF1HLR    = LAST USED FILE 1 RECORD
C        ZF1HNR    = NEXT AVAILABLE RELATION ROW
C        ZF1HNA    = NEXT AVAILABLE ATTRIBUTE ROW
C        ZF1HNL    = NEXT AVAILABLE LINK ROW
C
      PARAMETER (ZF1HLR=ZFXHTM+1)
      PARAMETER (ZF1HNR=ZF1HLR+1)
      PARAMETER (ZF1HNA=ZF1HNR+1)
      PARAMETER (ZF1HNL=ZF1HNA+1)
      PARAMETER (ZF1HDR=ZF1HNL)
C
C        ZF2HLR    = CURRENT FILE 2 RECORD FOR ADDING DATA
C        ZF1HNW    = NEXT AVAILABLE WORD FOR ADDING DATA
C
      PARAMETER (ZF2HLR=ZFXHTM+1)
      PARAMETER (ZF2HNW=ZF2HLR+1)
      PARAMETER (ZF2HDR=ZF2HNW)
C
C        ZF3HLR    = NEXT AVAILABLE RECORD FOR B-TREES
C        ZF3HMO    = CURRENT MOT RECORD
C        ZF3HNM    = NEXT AVAILABLE WORD IN CURRENT MOT RECORD
C        ZF3HRC    = START OF THE B-TREE FREE-RECORD CHAIN
C        ZF3HMC    = START OF THE B-TREE FREE-MOT CHAIN
C
      PARAMETER (ZF3HLR=ZFXHTM+1)
      PARAMETER (ZF3HMO=ZF3HLR+1)
      PARAMETER (ZF3HNM=ZF3HMO+1)
      PARAMETER (ZF3HRC=ZF3HNM+1)
      PARAMETER (ZF3HMC=ZF3HRC+1)
      PARAMETER (ZF3HDR=ZF3HMC)
C
C
C     ------------- RELATION, ATTRIBUTE, AND LINK TABLES -----------
C
C        ZRELL     = LENGTH OF RELATION ROW IN FILE 1
C        ZRELR     = NUMBER OF RELATION ROWS PER PAGE
C        ZRELRI    = INITIAL RELATION ROW
C        ZR1-ZR10  = POINTERS TO ITEMS IN RELATION ROW
C
      PARAMETER (ZRELL=7+3*Z)
      PARAMETER (ZRELR=ZF1/ZRELL)
      PARAMETER (ZRELRI=ZRELR+1)
      PARAMETER (ZR1=1)
      PARAMETER (ZR2=ZR1+1)
      PARAMETER (ZR3=ZR2+Z)
      PARAMETER (ZR4=ZR3+1)
      PARAMETER (ZR5=ZR4+1)
      PARAMETER (ZR6=ZR5+1)
      PARAMETER (ZR7=ZR6+1)
      PARAMETER (ZR8=ZR7+1)
      PARAMETER (ZR9=ZR8+1)
      PARAMETER (ZR10=ZR9+Z)
C
C
C        ZATTL     = LENGTH OF ATTRIBUTE ROW IN FILE 1
C        ZATTR     = NUMBER OF ATTRIBUTE ROWS PER PAGE
C        ZATTRI    = INITIAL ATTRIBUTE ROW
C        ZA1-ZA7   = POINTERS TO ITEMS IN ATTRIBUTE ROW
C
      PARAMETER (ZATTL=6+2*Z)
      PARAMETER (ZATTR=ZF1/ZATTL)
      PARAMETER (ZATTRI=(ZATTR*2)+1)
      PARAMETER (ZA1=1)
      PARAMETER (ZA2=ZA1+1)
      PARAMETER (ZA3=ZA2+Z)
      PARAMETER (ZA4=ZA3+Z)
      PARAMETER (ZA5=ZA4+1)
      PARAMETER (ZA6=ZA5+1)
      PARAMETER (ZA7=ZA6+1)
      PARAMETER (ZA8=ZA7+1)
C
C
C        ZLNKL     = LENGTH OF LINK ROW IN FILE 1
C        ZLNKR     = NUMBER OF LINK ROWS PER PAGE
C        ZLNKRI    = INITIAL LINK ROW
C        ZL1-ZL5   = POINTERS TO ITEMS IN LINK ROW
C
      PARAMETER (ZLNKL=1+5*Z)
      PARAMETER (ZLNKR=ZF1/ZLNKL)
      PARAMETER (ZLNKRI=(ZLNKR*3)+1)
      PARAMETER (ZL1=1)
      PARAMETER (ZL2=ZL1+1)
      PARAMETER (ZL3=ZL2+Z)
      PARAMETER (ZL4=ZL3+Z)
      PARAMETER (ZL5=ZL4+Z)
      PARAMETER (ZL6=ZL5+Z)
C
C
C     ------------- RIM COMMAND LIMITS -----------
C
C        ZMTOK     = MAX NUMBER OF INPUT TOKENS
C        ZMASC     = MAX LENGTH OF INPUT ASCII-TEXT
C        ZMSRT     = MAX SORT ATTRIBUTES
C        ZMSEL     = MAX SELECT ATTRIBUTES
C        ZMSLK     = MAX SELECT LINKS
C        ZMWHR     = MAX NUMBER OF WHERE CLAUSE ATTRIBUTES
C        ZMWHLL    = MAX NUMBER OF WHERE CLAUSE VALUE ITEMS
C        ZMWHVL    = MAX LENGTH OF WHERE CLAUSE VALUE ASCII-TEXT
C        ZMXMAC    = MAX NUMBER OF MACRO DEFINITIONS
C        ZMXMTX    = MAX WORDS OF MACRO DEFINITION TEXT 
C        ZMEOP     = MAX OPERATORS IN EXPRESSIONS (REPORT WRITER)
C
      PARAMETER (ZMTOK=200)
      PARAMETER (MAXCOL = ZMTOK)
      PARAMETER (ZMASC=ZMTOK*ZCW)
      PARAMETER (ZMSRT=10)
      PARAMETER (ZMSEL=35)
      PARAMETER (ZMSLK=5)
      PARAMETER (ZMWHR=15)
      PARAMETER (ZMWHVL=300,ZMWHLL=100)
      PARAMETER (ZMXMAC=50, ZMXMTX=500) 
      PARAMETER (ZMEOP=20)
C
C
C     ------------- BUFFER SIZES -----------
C
C        ZIOIN     = NUMBER SIMULTANEOUS INPUT FILES
C        ZIOOU     = NUMBER SIMULTANEOUS OUTPUT FILES
C        ZIOBF     = SIZE OF FILE BUFFERS
C        ZPIMAX    = MAX PROGRAMMABLE INTERFACE POINTER
C
      PARAMETER (ZIOIN=1)
      PARAMETER (ZIOOU=2)
      PARAMETER (ZIOBF=1)
      PARAMETER (ZPIMAX=9)
C
C        ZICBL     = NUMBER OF IN-CORE BLOCKS
C        ZBT       = BTBUF'S CORE ARRAY SIZE (ZICBL*ZF3)
C        ZBUF      = LENGTH OF /RIMCOM/ BUFFER AREA
C
      PARAMETER (ZBF4=484)
      PARAMETER (ZICBL=20)
      PARAMETER (ZBT=ZICBL*ZF3)
      PARAMETER (ZBUF=15000)
C
C
C     ------------- TEXT IO LIMITS -----------
C
C        ZCARDL       = MAX LENGTH OF CARD INPUT RECORDS
C        ZCARDW       = MAX LENGTH OF CARD INPUT RECORDS (WORDS)
C        ZCARDN       = NUMBER OF INPUT RECORDS FOR RECALL (FOR EDIT)
C        ZPRINL       = MAX LENGTH OF PRINTER OUTPUT RECORDS
C        ZPRINW       = MAX LENGTH OF PRINTER OUTPUT RECORDS (WORDS)
C
      PARAMETER (ZCARDL=512)
      PARAMETER (ZCARDW=(ZCARDL-1)/ZCW+1)
      PARAMETER (ZCARDN=10)
      PARAMETER (ZPRINL=512)
      PARAMETER (ZPRINW=(ZPRINL-1)/ZCW+1)
C
C        ZDATL        = LENGTH OF RIM DATE FIELDS
C        ZTIML        = LENGTH OF RIM TIME FIELDS
C
      PARAMETER (ZDATL=8,ZTIML=8)
C
C
C     ------------- I/O NAMES AND UNITS ----------
C
C        ZFNAML       = MAX FILENAME LENGTH
C        ZTRMIN       = TERMINAL INPUT FILENAME
C        ZTRMOU       = TERMINAL OUTPUT FILENAME
C        ZHFNAM       = HELP DB FILENAME
C
      PARAMETER (ZFNAML=100)
      CHARACTER*6 ZTRMIN, ZTRMOU
      PARAMETER (ZTRMIN='stdin ')
      PARAMETER (ZTRMOU='stdout')
      CHARACTER*(8) ZHFNAM
      PARAMETER (ZHFNAM='rim_help')
C
C
C        ZNINT        = UNIT FOR TERMINAL INPUT
C        ZNINTA       = UNIT FOR ALTERNATE TERMINAL INPUT
C        ZNOUT        = UNIT FOR TERMINAL OUTPUT
C        ZNOUTR       = UNIT FOR REPORT OUTPUT
C        ZNOUTL       = UNIT FOR LOG OUTPUT
C        ZNOUTT       = UNIT FOR TRACE OUTPUT
C
C        ZNFIL1       = UNIT FOR DATABASE FILE 1
C        ZNFIL2       = UNIT FOR DATABASE FILE 2
C        ZNFIL3       = UNIT FOR DATABASE FILE 3
C
C        ZNSRT        = UNITS FOR SORTING
C                       (ALLOCATES ZNSRT THROUGH ZNSRT+ZPIMAX)
C        ZNSRT1       = SORT SCRATCH UNIT
C        ZCSRT1       = SORT SCRATCH NAME
C        ZNSRT2       = SORT SCRATCH UNIT
C        ZCSRT2       = SORT SCRATCH NAME
C        ZFSORT       = LENGTH OF SORT FILE BLOCKS
C
      PARAMETER (ZNINT=5, ZNINTA=4)
      PARAMETER (ZNOUT=6, ZNOUTR=7, ZNOUTL=8, ZNOUTT=9)
      PARAMETER (ZNFIL1=31, ZNFIL2=ZNFIL1+1, ZNFIL3=ZNFIL1+2)
      PARAMETER (ZNSRT=40)
      PARAMETER (ZNSRT1=38, ZNSRT2=39)
      CHARACTER*(ZFNAML) ZCSRT1, ZCSRT2
      PARAMETER (ZCSRT1='RIM.SCRATCH1')
      PARAMETER (ZCSRT2='RIM.SCRATCH2')
      PARAMETER (ZFSORT=1024)
C
C
C     ------------- SPECIAL NUMBERS ------------
C
C        NULL VALUES ('MISSING' AND 'NOT APPLICABLE')
C
C        ZIMISS       = MISSING VALUE FOR INTEGERS
C        ZRMISS       = MISSING VALUE FOR REALS (NOTE TYPE INTEGER)
C        ZTMISS       = MISSING VALUE FOR TEXT
C
C        ZINAPP       = NOT APPLICABLE FOR INTEGERS
C        ZRNAPP       = NOT APPLICABLE FOR REALS (NOTE TYPE INTEGER)
C        ZTNAPP       = NOT APPLICABLE FOR TEXT
C
C        ZTEND        = FLAG FOR END OF KEY CHAINS
C                       (GREATER THAN INFINITY)
C        ENDWRD       = ZTEND
C        ALL9S        = INFINITY
C                       (THIS IS MAXIMUM ALLOWABLE INTEGER + 1)
C
C        MIN9S        = MINUS INFINITY
C                       (THIS IS MINIMUM ALLOWABLE INTEGER - 1)
C
C        NULL         = AN ALTERNATE MISSING VALUE
C
C        BY COINCIDENCE (?) THE NULLS ARE ALL THE SAME ON MOST MACHINES
C        AS ARE THE NOT APPLICABLES
C        NOTE.. 2 AVAILABLE INTEGERS FOR FLAGS
C
      PARAMETER (ZIMISS=2147483647)
      PARAMETER (ZRMISS=ZIMISS, ZTMISS=ZIMISS)
      PARAMETER (NULL=ZIMISS)
      PARAMETER (ZINAPP=ZIMISS-1)
      PARAMETER (ZRNAPP=ZINAPP, ZTNAPP=ZINAPP)
      PARAMETER (ZTEND=ZIMISS-4)
      PARAMETER (ENDWRD=ZTEND)
      PARAMETER (ALL9S=ZIMISS-5)
      PARAMETER (MIN9S=0-ALL9S)
C
C        ZHALF        = HALFWORD
C        ZHTOI        = PACKING FACTOR USED BY ITOH/HTOI
C                       (NOTE.. REMAINDER OF THE WORD MUST BE
C                        LARGER THAN LARGEST FILE BLOCK SIZE)
C
      PARAMETER (ZHALF=2**16)
      PARAMETER (ZHTOI=2**20)
C