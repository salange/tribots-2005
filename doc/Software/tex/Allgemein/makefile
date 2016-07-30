TARGET = tribots_doku
TEXS = $(TARGET).tex bedienung.tex grundstruktur.tex tools.tex komponenten.tex
FIGS = datenfluss.fig komponenten.fig zeitablauf.fig
EPS = $(FIGS:.fig=.eps)
AUXS = $(TEXS:.tex=.aux)

$(TARGET).ps : $(TARGET).dvi
	dvips $(TARGET).dvi

$(TARGET).dvi : $(EPS) $(TEXS)
	latex $(TARGET)
	latex $(TARGET)

%.eps : %.fig
	fig2dev -Leps $< $@

clean:
	\rm $(EPS) $(TARGET).dvi $(TARGET).ps $(TARGET).log $(TARGET).toc $(AUXS)

