TARGET = WorldModel
FIGS = hierarchy.fig sensorwahrscheinlichkeit.fig error.fig referee_states.fig

EPS = $(FIGS:.fig=.eps)

$(TARGET).dvi : $(EPS) $(TARGET).tex
	latex $(TARGET)
	latex $(TARGET)

%.eps : %.fig
	fig2dev -Leps $< $@

clean:
	\rm $(TARGET).dvi $(TRAGET).ps $(TRAGET).pdf $(EPS) $(TARGET).aux $(TARGET).log

