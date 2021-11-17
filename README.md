# Raccolta di macro per il framework root (Cern)
Raccolta di macro scritte durante il corso di analisi dati (terzo semestre magistrale).

Le macro sono quelle passate dal professore durante le lezioni, anche presenti sul sito [del corso](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/scientific-data-analysis-laboratory-2021-2022.html),
alle quali ho applicato alcune modifiche. (Principalmente stilistiche e di refactoring).

Ogni cartella è un'esercitazione. Ogni cartella contiene un file .C (la macro), una cartella chiamata root_files, in cui vengono letti e scritti i dati,
e una cartella Plots, in cui vengono salvati i plot generati dalle macro.


**TODO** Inserire breve descrizione di ogni esercitazione e pdf

# Es 1: Stacked plot
[Pdf dell'esercitazione](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/esercitazione-1.pdf)
**Finito**

# Es 2: Cms vs Alice
**Qui non c'è un pdf dell'esercitaione, ci siamo inventati noi la macro cercando di fare i plot**
[Cms vs FONLL](https://arxiv.org/pdf/2107.01476v1.pdf) -> figura 5 a pagina 13.
Cms vs Alice da un pptx suo che non sta sul sito.

Divisa in due parti. Nella prima [cms_vs_alice_solo_cms](./es2_cms_vs_alice_D0/cms_vs_alice_solo_cms) c'è solo il plot dei dati di cms e del rapporto tra MC e dati in cms.
Nella seconda cartella [cms_vs_alice_completa](./es2_cms_vs_alice_D0/cms_vs_alice_completa) c'è l'esercitazione completa: Ci sono i grafici di alice/cms, cms/alice, (cms-alice)/cms
e cms vs alice (che sarebbe il confronto tra i dati). **Grazie a Nicola che questa l'ha completata lui**
**Finito**

# Es 3: Curva roc e test delle ipotesi
[Pdf dell'esercitazione](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/lezione-roc-higgs-4lept.pdf).

# Es 4: Fit con roofit

## Es 4a: Fit con gaussiana + fondo con roofit
[Pdf dell'esercitazione](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/esercitazione-roofit-invmass.pdf).
[Pdf su Migrad, Hesse e Minos](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/esercitazione-roofit-invmass.pdf).

## Es 4b: Pull
[Pdf dell'esercitazione](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/Exercise3b.pdf)

## Es 5: Fit spettro J/#Psi
[Pdf](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/Exercise4-outline.pdf)

## Es 6: Fit con pull (esercitazione con esame 2017)
[Pdf](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/Traccia-esame-28marzo17.pdf)
**Finito**

## Es 7: Uso dei dati di Es 4 per fittare tutti i bin (da 1 a 23)
Bisogna riprendere il fit fatto nella Es 4 e questa volta fittare in una volta sola tutti gli istogrammi (da PsiPrimeMass_bin1 a PsiPrimeMass_bin23) e fare un TGraphErrors dove inserire le medie e le sigma fittate.
Vedere se si riesce a fittare la distribuzione che ne deriva.

# Note sulla versione di CINT e root.
Versione di root locale: 6.22/02 (built from tag, 17 August 2020).
Versione di root nella macchina remota: 6.14/09 (built from tag, 22 November 2018).

Sono stati trovati alcuni bug nella versione precedente di root. Verranno qui segnalati. I bug non sono stati esplorati, ci si è limitati ad evitarli.

- La classe TH1D * non viene trovata nell'albero di discendenza della classe TObject *. Questo provoca un non funzionamento del dynamic_cast dagli oggetti estratti dai root file ad istogrammi. Per questo
motivo è stato spesso necessario fare conversioni c-like. Probabilmente basterebbe utilizzare static_cast ma non è stato ancora verificato. Inoltre non si è indagato su quali classi non vengono trovate
nell'albero oltre a TH1D *
