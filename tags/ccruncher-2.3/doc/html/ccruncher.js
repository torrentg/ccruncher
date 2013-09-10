
function adaptBackground() 
{
	if (navigator.userAgent.match(/iPad/i) || 
	    navigator.userAgent.match(/iPhone/i) ||
	    navigator.userAgent.match(/Android/i) ||
	    navigator.userAgent.match(/BlackBerry/i) ||
	    navigator.userAgent.match(/webOS/i)
	   )
	{
		document.body.style.backgroundColor="white";
		document.body.style.backgroundImage="none";
	}
}

