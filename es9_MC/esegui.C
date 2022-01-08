void esegui() {
    TString indici[] = {
        "50000",
        "100000"
    };

    for (int i{0}; i < 2; ++i) {
        RooConvolutionExpNew(indici[i], "yes", 200);
    }
}
