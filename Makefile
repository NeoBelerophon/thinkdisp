all:
	gcc scripts/src/killdisp.c -lX11 -lXrandr -o scripts/killdisp
