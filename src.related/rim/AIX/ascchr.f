      INTEGER FUNCTION ASCCHR(CH)
      INCLUDE 'syspar.d'
C
C     ** UNIX SYSTEM DEPENDENT INTERNAL ROUTINE **
C
C     RETURN THE ASCII-CHAR EQUIVALENT OF CH
C
      CHARACTER*1 CH
C
      ASCCHR = ICHAR(CH)
C     CONVERT TAB (9) TO SPACE (32)
      IF (ASCCHR.EQ.9) ASCCHR = 32
      RETURN
      END