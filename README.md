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

## NOTA: sulla conversione tra stringhe c++ e stringhe c-like.
In questa macro di presenta la necessità di creare una stringa fatta così "hVar1Ratio[i]" dove al posto della i devo mettere il numero. Quindi è necessario fare un'operazione complessa di gestione e concatenazione di stringhe. Questo è facile con i metodi delle stringhe della libreria std. Il costruttore di TH1D (e in generale tutti i metodi che in root vogliono le stringhe), però, prende un tipo const char *, cioè una stringa c-like, o al più una TString.
Per risolvere il problema lui faceva così
```cpp
char histoname[128];
sprintf(histoname,"hVarRatio[%d]",i);
```
Questo metodo funziona bene per il caso specifico, ma è c-like: in pratica creo un buffer di caratteri e stampo dentro il testo che mi serve usando i segnaposti da associare alle variabili.
Inoltre, con i buffer di caratteri non sono in grado di fare operazioni più complesse (tipo cambiare il case delle lettere eccetera), questo perchè l'oggetto di tipo char[] non è una classe e quindi è stateless, non può avere metodi.
C++ mette a disposizione la classe built-in `<string>` contenuta nella std library. Inoltre fornisce tutti gli strumenti per la retrocompatibilità con le stringhe c-like.
```cpp
std::string nomeHisto{"hVar" + std::to_string(varValue + 1) + "Ratio[" + std::to_string(i) + "]"};
const char *nomeHistoChar = nomeHisto.c_str(); // Converte la stringa c++ in stringa c-like
```
Quindi creo una variabile di tipo string e la inizializzo con il nome che voglio utilizzando tranquillamente l'operatore + tra `const char*` (cioè le stringhe letterali) e `string`, restituita dal metodo `to_string()` che permette la concatenazione e restituisce una `string` e uso la stringa restituita dall'operazione come argomento del costruttore della variabile di tipo `string` nomeHisto. Ho necessità, però, di una stringa c-like, quindi effettuo la conversione usando il metodo delle stringhe `c_str()`. Che restituisce un const char*.

**Finito**

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
**Finito ma il fit viene un po' a schifo a me (a Nicola no). Capire perchè.**

## Es 8: Fit decadimento Phi
[Pdf](https://www.ba.infn.it/~pompili/teaching/data_analysis_lab/Fit-to-Phi-mass-withVoigtian.pdf)

# Note sulla versione di CINT e root.
Versione di root locale: 6.22/02 (built from tag, 17 August 2020).
Versione di root nella macchina remota: 6.14/09 (built from tag, 22 November 2018).

Sono stati trovati alcuni bug nella versione precedente di root. Verranno qui segnalati. I bug non sono stati esplorati, ci si è limitati ad evitarli.

- La classe TH1D * non viene trovata nell'albero di discendenza della classe TObject *. Questo provoca un non funzionamento del dynamic_cast dagli oggetti estratti dai root file ad istogrammi. Per questo
motivo è stato spesso necessario fare conversioni c-like. Probabilmente basterebbe utilizzare static_cast ma non è stato ancora verificato. Inoltre non si è indagato su quali classi non vengono trovate
nell'albero oltre a TH1D *
