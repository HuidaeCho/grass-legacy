      SUBROUTINE ZMOVE(WORD1,WORD2)
      INCLUDE 'syspar.d'
C
C  PURPOSE:   COPIES WORD2 TO WORD1
C
C  PARAMETERS:
C         WORD1---DESTINATION LONG WORD
C         WORD2---SOURCE LONG WORD
C
      INTEGER WORD1(Z),WORD2(Z)
C
      DO 100 I = 1, Z
100   WORD1(I) = WORD2(I)
      RETURN
      END
