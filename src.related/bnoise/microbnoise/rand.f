C$NOFLOATCALLS
	SUBROUTINE RAND(IY,FMX,RNOPT,NRN,X)
	DIMENSION X(NRN)
	DATA R/.31415926/
	DO 100  I=1,NRN
	BB=1.
	P1=R*317.
	R=AMOD(P1,BB)
100	X(I)=R
	RETURN
	END
