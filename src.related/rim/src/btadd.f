      SUBROUTINE BTADD(VALU,IPTR,TYPE)
      INCLUDE 'syspar.d'
C
C  PURPOSE:   ADD NEW VALUES TO A BTREE
C
C  PARAMETERS
C    INPUT:  VALU----KEY VALUE TO PROCESS
C            IPTR----POINTER TO TUPLE HAVING THIS KEY VALUE
C            TYPE----TYPE OF VARIABLE BEING ADDED
C
C  SUBROUTINES USED
C         BTGET---PAGING ROUTINE
C         BTSERT--USED TO INSERT VALUES IN A BTREE
C         BTPUT---PAGING ROUTINE
C
      INCLUDE 'ascpar.d'
      INCLUDE 'rmatts.d'
      INCLUDE 'f3com.d'
      INCLUDE 'rimcom.d'
      INCLUDE 'btbuf.d'
      INCLUDE 'start.d'
      INCLUDE 'stack.d'
C
C     RVAL IS REAL UNTIL BTREE IS ALLOWED 2 WORDS PER ENTRY
C
      INTEGER VAL,VALT,VALU(1)
      REAL RVAL
      EQUIVALENCE (RVAL,VAL)
      INTEGER TYPE
C
C  INITIAL START OF THE SCAN.
C
      SP = 0
      KSTART = START
      VAL = VALU(1)
      IF (TYPE.EQ.KZREAL) VAL = DTOR(VALU)
      ITYPE = TYPE
      IF(TYPE.EQ.KZDATE) ITYPE = KZINT
      IF(TYPE.EQ.KZTIME) ITYPE = KZINT
      IF(TYPE.EQ.KZTEXT) ITYPE = KZINT
      IP = IPTR
  100 CONTINUE
      SP = SP + 1
      STACK(SP) = KSTART
C
C  FETCH A NODE.
C
      CALL BTGET(KSTART,IN)
      KEND = IN + (LENBF3/3) - 1
C
C  LOOP THROUGH A NODE.
C
      DO 300 J=IN,KEND
C
C  CHECK FOR END-OF-LIST WORD.
C
      IF(VALUE(1,J).EQ.ENDWRD) GO TO 200
C
C  IF THE VALUE IS LT VAL THEN KEEP LOOKING.
C
      IF((ITYPE.EQ.KZINT).AND.(VALUE(1,J).LT.VAL)) GO TO 300
      IF((ITYPE.NE.KZINT).AND.(RVALUE(1,J).LT.RVAL)) GO TO 300
C
C  FOUND A BIGGER VALUE.
C
  200 CONTINUE
C
C  GO TO THE NEXT BRANCH IF THERE IS ONE.
C
      IF(VALUE(2,J).GE.0) GO TO 400
      KSTART = -VALUE(2,J)
      GO TO 100
  300 CONTINUE
C
C  WE DID NOT FIND THE END-OF-LIST WORD. DISASTER.
C
      GO TO 1000
C
C  ADD IT BETWEEN EXISTING VALUES.
C
  400 CONTINUE
C
C  CHECK FOR A DUPLICATE VALUE.
C
      IF(VALUE(1,J).NE.VAL) GO TO 500
C
C  WE HAVE A MULTIPLE VALUE. SEE IF THIS IS THE FIRST DUPLICATE.
C
      IF(VALUE(3,J).NE.0) GO TO 420
C
C  DO SPECIAL PROCESSING FOR THE FIRST MULTIPLE VALUE.
C
      IPTR1 = VALUE(2,J)
      IF(MOTADD.LT.LENBF3) GO TO 410
      MOTADD = 0
      MOTREC = LF3REC
      LF3REC = LF3REC + 1
  410 CONTINUE
      CALL HTOI(MOTADD+1,MOTREC,KWORD)
      VALUE(3,J) = KWORD
      VALUE(2,J) = KWORD
      CALL BTGET(STACK(SP),IN)
      CALL BTPUT(STACK(SP))
C
C  ADD THE FIRST LINK TO THE MOT.
C
      CALL BTGET(MOTREC,IN)
      MOTIND = 3 * IN - 3
      MOTADD = MOTADD + 1
      MOTIND = MOTIND + MOTADD
      CORE(MOTIND+1) = IPTR1
      MOTADD = MOTADD + 1
      CALL BTPUT(MOTREC)
  420 CONTINUE
C
C  FIX UP THE END POINTER.
C
      IF(MOTADD.LT.LENBF3) GO TO 430
      MOTADD = 0
      MOTREC = LF3REC
      LF3REC = LF3REC + 1
  430 CONTINUE
      CALL ITOH(MOTIND,MOTID,VALUE(2,J))
      CALL HTOI(MOTADD+1,MOTREC,VALUE(2,J))
      CALL BTGET(STACK(SP),IN)
      CALL BTPUT(STACK(SP))
C
C  GET THE END OF THE MOT TRAIL.
C
      CALL BTGET(MOTID,IN)
      IN = 3 * IN - 3
      MOTIND = MOTIND + IN
C
C  ADD THE NEXT LINK IN THE MOT.
C
      MOTADD = MOTADD + 1
      CALL HTOI(MOTADD,MOTREC,KWORD)
      CORE(MOTIND) = KWORD
      CALL BTPUT(MOTID)
C
C  NOW ADD THE POINTER TO THE MOT.
C
      CALL BTGET(MOTREC,IN)
      IN = 3 * IN - 3
      MOTADD = MOTADD + 1
      MOTIND = IN + MOTADD
      CORE(MOTIND) = IPTR
      CALL BTPUT(MOTREC)
      RETURN
C
C  THIS VALUE IS NOT IN THE BTREE YET.
C
  500 CONTINUE
C
C  CALL BTSERT TO INSERT THE DATA.
C
      VALT = VAL
      IPT = IP
  600 CONTINUE
      CALL BTSERT(VALT,IPT,STACK,SP,J,IN)
      IF(SP.EQ.0) RETURN
C
C  FETCH THE NEXT NODE UP THE STACK.
C
      CALL BTGET(STACK(SP),IN)
C
C  CALCULATE A NEW VALUE FOR J.
C
      KEND = IN + (LENBF3/3) - 1
      DO 700 J=IN,KEND
      IF(VALUE(1,J).EQ.ENDWRD) GO TO 600
      IF((ITYPE.EQ.KZINT).AND.(VALUE(1,J).LT.VAL)) GO TO 700
      IF((ITYPE.NE.KZINT).AND.(RVALUE(1,J).LT.RVAL)) GO TO 700
C
C  WE FOUND A BIGGER VALUE.
C
      GO TO 600
  700 CONTINUE
C
C  SOMETHING IS WRONG. WE CANNOT FIND A LARGER VALUE.
C
      RMSTAT = 1003
      RETURN
C
C  LOOKUP FOR A VALUE NOT IN THE TREE.
C
 1000 CONTINUE
      RETURN
      END
