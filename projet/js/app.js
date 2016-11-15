window.onload = function()
{
    var canvasOriginal = document.getElementById('original');
    if(!canvasOriginal)
    {
        alert("Impossible de récupérer le canvas");
        return;
    }

    var contextOriginal = canvasOriginal.getContext('2d');
    if(!contextOriginal)
    {
        alert("Impossible de récupérer le context du canvas");
        return;
    }

    var canvasResult = document.getElementById('original');
    if(!canvasResult)
    {
        alert("Impossible de récupérer le canvas");
        return;
    }

    var contextResult = canvasResult.getContext('2d');
    if(!contextResult)
    {
        alert("Impossible de récupérer le context du canvas");
        return;
    }


    //C'est ici que l'on placera tout le code servant à nos dessins.
}