window.addEventListener('load', () => init());
async function init()
{
	let token = sessionStorage.getItem('token');
	
	let startB = document.getElementById("blackjackStartButton");
	let stopB = document.getElementById("blackjackStopButton");	
	let betB = document.getElementById("blackjackBetForm");
	
	let startD = document.getElementById("blackjackStart");
	let betD = document.getElementById("blackjackBet");
	let stopD = document.getElementById("blackjackStop");

	let handD = document.getElementById("blackjackHands");
	let dhandD = document.getElementById("blackjackDealerHand");
	let phandD = document.getElementById("blackjackPlayerHand");
	
	p = await getPhase();
	console.log("phase: " + p);
	if(p == -1)
	{
		startD.style.display = "block";
		betD.style.display = "none";
		stopD.style.display = "none";
		handD.style.display = "none";
	}
	if(p == 0)
	{
		startD.style.display = "none";
		betD.style.display = "block";
		stopD.style.display = "none";
		handD.style.display = "none";
	}
	if(p == 1)
	{
		startD.style.display = "none";
		betD.style.display = "none";
		handD.style.display = "block";
		stopD.style.display = "none";
	}

	startB.addEventListener("click", () => 
	{		
		post({"join":token}); 
		post({"start":token});
		startD.style.display = "none";
		betD.style.display = "block";
	}, false);

	stopB.addEventListener("click", () => post({"stop":token}), false);	
	betB.addEventListener("submit", (event) => betAmount(event));
}

async function post(data){
	const response = await fetch("/blackjack",{method: 'POST',body: JSON.stringify(data)})
	.then(response => response.text())
	.catch(error => console.log(error));
	
	console.log("response : " + await response);
}

async function get(url)
{
	const response = await fetch(url,{method: 'GET'})
	.then(response => response.text())
	.catch(error => console.log(error));
	console.log(await response);
}

async function betAmount(event)
{
	event.preventDefault();
	
	let formData = new URLSearchParams(new FormData(document.getElementById("blackjackBetForm")));

	let url = "/blackjack";
	let data = {
		"bet": formData.get("betAmount").toString(),
		"token": sessionStorage.getItem('token')
	}
	
	let isOK = false;
	
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( function(response) 
		{
			if(response.toString() == "OK") 
			{
				console.log("BET_PLACED");
				isOK = true;
			}
			else
			{
				document.getElementById("errorResponse").innerHTML = response.toString();
			}
		}
	)
	.catch(error => console.log(error));
	
	if(isOK)
	{
		getHands();
	}
}

async function getHands()
{
	let url = "/blackjack";
	let data = {
		"gethands": sessionStorage.getItem('token')
	}
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( response => hands = response.toString() )
	.catch(error => console.log(error));

	console.log("hands : " + await response);
}

async function getPhase()
{
	let phase = -1;
	let url = "/blackjack";
	let data = {
		"getPhase": sessionStorage.getItem('token')
	}
	const phaseResponse = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( function(response) 
		{
			if(parseInt(response) >= 0) 
			{
				phase = parseInt(response);
			}
		}
	).catch(error => console.log(error));
	return phase;
}