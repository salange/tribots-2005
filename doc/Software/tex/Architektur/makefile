TARGET = architektur
FIGS = datenfluss.fig komponenten.fig sandwichschnittstelle.fig feldkoordinaten.fig roboterkoordinatensystem.fig
EPS = $(FIGS:.fig=.eps)

$(TARGET).dvi : $(EPS) $(TARGET).tex
	latex $(TARGET)
	latex $(TARGET)

%.eps : %.fig
	fig2dev -Leps $< $@

clean:
	\rm $(EPS) $(TARGET).dvi $(TRAGET).ps $(TRAGET).pdf $(TRAGET).aux $(TRAGET).log

datenfluss.eps : datenfluss.fig
komponenten.eps : komponenten.fig
sandwichschnittstelle.eps : sandwichschnittstelle.fig
feldkoordinaten.eps : feldkoordinaten.fig
roboterkoordinatensystem.eps : roboterkoordinatensystem.fig
