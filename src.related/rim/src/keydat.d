C
C  *** / K E Y D A T / ***
C
C  KEY ATTRIBUTE DATA FOR THE FORTRAN INTERFACE
C
      PARAMETER (ZPIKAT=10)
      PARAMETER (ZKEYDA=4+Z)
C
      COMMON /KEYDAT/ NUMKEY,KEYDAT(ZKEYDA,ZPIKAT)
C
C  VARIABLE DEFINITIONS:
C         NUMKEY--NUMBER OF KEY ATTRIBUTES (CURRENT RELATION)
C         KEYDAT--ARRAY OF KEY ATTRIBUTE DATA
C                       ROW 1 - ATTKEY VALUES
C                       ROW 2 - ATTCOL VALUES
C                       ROW 3 - ATTWDS VALUES
C                       ROW 4 - ATTYPE VALUES
C                       ROW 5 - ATTNAM VALUES
C