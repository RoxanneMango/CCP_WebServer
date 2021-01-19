var betPlaced = false;
var insurancePlaced = false;

window.addEventListener('load', () => init());
async function init()
{
	let token = sessionStorage.getItem('token');
	
	let startD = document.getElementById("blackjackStart");
	let betD = document.getElementById("blackjackBet");
	let insuranceD = document.getElementById("blackjackInsurance");
	let insuranceF = document.getElementById("blackjackInsureForm");
	let splitD = document.getElementById("blackjackSplit");
	let handD = document.getElementById("blackjackHands");
	let dhandD = document.getElementById("blackjackDealerHand");
	let phandD = document.getElementById("blackjackPlayerHand");
	let hitD = document.getElementById("blackjackHit");
	let stopD = document.getElementById("blackjackStop");
	
	p = await getPhase();
	if(p == -1)
		startD.style.display = "block";
	if(p == 0)
		betD.style.display = "block";
	if(p >= 1)
	{
		getHands();
		handD.style.display = "block";
		if(p == 2)
			insuranceD.style.display = "block";
		else if(p == 3)
			splitD.style.display = "block";
		else if(p == 4)
			hitD.style.display = "block";
		else
		{
			stopD.style.display = "block";
			await getSettlement();
		}
	}
	document.getElementById("blackjackStartButton").addEventListener("click", async function() 
	{		
		await post({"join":token}); 
		await post({"start":token});
		startD.style.display = "none";
		betD.style.display = "block";
	}, false);
	document.getElementById("blackjackBetForm").addEventListener("submit", async function(event) 
	{		
		console.log(await betAmount(event));
		if(await betAmount(event) == true)
		{
			betD.style.display = "none";
			handD.style.display = "block";
			await getBalance();
			await getHands();
			await getSettlement();
			let p = await getPhase();
			if(p == 2)
				insuranceD.style.display = "block";
			else if(p == 3)
				splitD.style.display = "block";
			else if(p == 4)
				hitD.style.display = "block";
			else
				stopD.style.display = "block";

		}
	}, false);
	document.getElementById("blackjackInsuranceButtonYes").addEventListener("click", async function(event) 
	{
		insuranceD.style.display = "none";
		insuranceF.style.display = "block";
	}, false);
	document.getElementById("blackjackInsuranceButtonNo").addEventListener("click", async function(event) 
	{
		await post({"continue":token});
		await getHands();
		let p = await getPhase();
			if(p == 3)
				splitD.style.display = "block";
			else if(p == 4)
				hitD.style.display = "block";
			else
				stopD.style.display = "block";
		insuranceD.style.display = "none";
		console.log("phase:" + p);
	}, false);
	document.getElementById("blackjackInsureForm").addEventListener("submit", async function(event) 
	{		
		if(await insureAmount(event) == true)
		{
			insuranceF.style.display = "none";
			await getBalance();
			await getHands();
			await getSettlement();
			let p = await getPhase();
				if(p == 3)
					splitD.style.display = "block";
				else if(p == 4)
					hitD.style.display = "block";
				else
					stopD.style.display = "block";
		}
	}, false);
	document.getElementById("blackjackStandButton").addEventListener("click", async function(event)
	{
		await post({"stand":token});
		await getHands();
		if(await getSettlement() >= 0)
		{
			console.log("SETTLED!\n");
			await getBalance();
			hitD.style.display = "none";
			stopD.style.display = "block";
		}
	}, false);
	document.getElementById("blackjackHitButton").addEventListener("click", async function(event)
	{
		await post({"hit":token}); 
		await getHands();
		if(await getSettlement() >= 0)
		{
			await getBalance();
			console.log("SETTLED!\n");
			hitD.style.display = "none";
			stopD.style.display = "block";
		}
	}, false);
	document.getElementById("blackjackDoubleDownButton").addEventListener("click", async function(event)
	{
		const doubleDown = await post({"doubledown":token});
		if(doubleDown == "DOUBLE_DOWN")
		{
			await getHands();
			await getBalance();
			if(await getSettlement() >= 0)
			{
				console.log("SETTLED!\n");
				hitD.style.display = "none";
				stopD.style.display = "block";
			}
		}
		else
		{
			document.getElementById("errorResponseBlackjack").innerHTML = await doubleDown.toString();
		}
	}, false);
	document.getElementById("blackjackSurrenderButton").addEventListener("click", async function(event)
	{
		await post({"surrender":token}); 
		await getHands();
		if(await getSettlement() >= 0)
		{
			console.log("SETTLED!\n");
			hitD.style.display = "none";
			stopD.style.display = "block";
		}
	}, false);
	document.getElementById("blackjackRetryButton").addEventListener("click", async function()
	{
		location.reload();
	}, false);
	document.getElementById("blackjackStopButton").addEventListener("click", async function()
	{
		await post({"stop":token});
	}, false);
	document.getElementById("blackjackSplitButtonYes").addEventListener("click", async function()
	{
		const split = await post({"split":token});
		if(split == "OK")
		{
			await getHands();
			await getBalance();
			splitD.style.display = "none";
			hitD.style.display = "block";
		}
		else
		{
			document.getElementById("errorResponseBlackjack").innerHTML = await split.toString();
		}
	}, false);
	document.getElementById("blackjackSplitButtonNo").addEventListener("click", async function(event) 
	{
		await post({"continue":token});
		splitD.style.display = "none";
		hitD.style.display = "block";
	}, false);
}
async function post(data){
	const response = await fetch("/blackjack",{method: 'POST',body: JSON.stringify(data)})
	.then(response => response.text())
	.catch(error => console.log(error));
	console.log("response : " + await response);
	document.getElementById("errorResponseBlackjack").innerHTML = "";
	return await response;
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
	if(betPlaced)
	{
		return true;
	}
	event.preventDefault();
	document.getElementById("errorResponseBlackjack").innerHTML = "";
	let formData = new URLSearchParams(new FormData(document.getElementById("blackjackBetForm")));
	let url = "/blackjack";
	let data = {
		"bet": sessionStorage.getItem('token'),
		"amount": formData.get("betAmount").toString()
	}
	let isOK = false;
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( async function(response) 
		{
			if(response.toString() == "OK") 
			{
				console.log("BET_PLACED");
				betPlaced = true;
				isOK = true;
			}
			else
			{
				document.getElementById("errorResponseBlackjack").innerHTML = response.toString();
			}
		}
	)
	.catch(error => console.log(error));
	return isOK;
}

async function insureAmount(event)
{
	event.preventDefault();
	if(insurancePlaced)
	{
		return true;
	}
	document.getElementById("errorResponseBlackjack").innerHTML = "";	
	let formData = new URLSearchParams(new FormData(document.getElementById("blackjackInsureForm")));
	let url = "/blackjack";
	let data = {
		"insure": sessionStorage.getItem('token'),
		"amount": formData.get("insureAmount").toString()
	}
	
	let isOK = false;
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( async function(response) 
		{
			if(response.toString() == "OK") 
			{
				console.log("INSURANCE_PLACED");
				insurancePlaced = true;
				isOK = true;
			}
			else
			{
				document.getElementById("errorResponseBlackjack").innerHTML = response.toString();
			}
		}
	)
	.catch(error => console.log(error));
	return isOK;
}

async function getHands()
{
	let url = "/blackjack";
	let data = {
		"gethands": sessionStorage.getItem('token')
	}
	const string = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.catch(error => console.log(error));
	
	let turn = await fetch(url, {method: 'POST', body: JSON.stringify({"getturn":sessionStorage.getItem('token')})})
	.then(response => response.text())
	.catch(error => console.log(error));
	
	let hands = string.split(";");
	let playerHand = hands[0].split(":");
	let playerHand2;
	let dealerHand;
	console.log("Hands size: " + hands.length);
	if(hands.length > 3)
	{
		playerHand2 = hands[1].split(":");
		dealerHand = hands[2].split(":");
	}
	else
	{
		dealerHand = hands[1].split(":");
	}
	document.getElementById("dealerName").innerText = "Dealer";
	document.getElementById("playerName").innerText = "You";
	document.getElementById("blackjackDealerHand").innerHTML = "";
	document.getElementById("blackjackPlayerHand").innerHTML = "";
	document.getElementById("blackjackPlayerHand2").innerHTML = "";
	let dealerCards = [];
	let playerCards = [];
	for(let i = 1; i < dealerHand.length; ++i)
	{
		let temp = dealerHand[i].split(",");
		if(temp.length == 3)
		{
			let suit = temp[1] == "H" ? "\u2665" : temp[1] == "S" ? "\u2660" : temp[1] == "D" ? "\u2666" : "\u2663";
			document.getElementById("blackjackDealerHand").innerHTML += `<div class="card"><div class="cardValue">` + temp[0] + `</div><div class="cardSuit">` + suit + '</div><div class="cardValue mirror">' + temp[0] + `</div></div>`;
		}
		else if(temp.length = 1)
		{
			document.getElementById("blackjackDealerHand").innerHTML += `<div class="card hidden"><div class="cardValue" style="color:#0c2b0e;">?</div><div class="cardSuit" style="color:#0c2b0e;">?</div><div class="cardValue mirror" style="color:#0c2b0e;">?</div></div>`;
		}
		console.log("temp = " + temp + " ; length = " + temp.length);
	}
	if(await turn == 0)
	{
		document.getElementById("blackjackPlayerHand").innerHTML += `<h4 class="turnIndicator">~></h4>`;
	}
	for(let i = 1; i < playerHand.length; ++i)
	{
		let temp = playerHand[i].split(",");
		if(temp.length == 3)
		{
			let suit = temp[1] == "H" ? "\u2665" : temp[1] == "S" ? "\u2660" : temp[1] == "D" ? "\u2666" : "\u2663";
			document.getElementById("blackjackPlayerHand").innerHTML += `<div class="card"><div class="cardValue">` + temp[0] + `</div><div class="cardSuit">` + suit + '</div><div class="cardValue mirror">' + temp[0] + `</div></div>`;
		}
		else if(temp.length = 1)
		{
			document.getElementById("blackjackPlayerHand").innerHTML += `<div class="hiddenCard"></div>`;
		}
	}
	if(await turn == 0)
	{
		document.getElementById("blackjackPlayerHand").innerHTML += `<h4 class="turnIndicator"><~</h4>`;
	}
	if(hands.length > 3)
	{
		document.getElementById("blackjackPlayerHand2").innerHTML += `<hr class="divider" style="margin:1vw;border-color:#363636;width:20vw;">`;
		if(await turn > 0)
		{
			document.getElementById("blackjackPlayerHand2").innerHTML += `<h4 class="turnIndicator">~></h4>`;
		}
		for(let i = 0; i < playerHand2.length; ++i)
		{
			let temp = playerHand2[i].split(",");
			if(temp.length == 3)
			{
				let suit = temp[1] == "H" ? "\u2665" : temp[1] == "S" ? "\u2660" : temp[1] == "D" ? "\u2666" : "\u2663";
				document.getElementById("blackjackPlayerHand2").innerHTML += `<div class="card"><div class="cardValue">` + temp[0] + `</div><div class="cardSuit">` + suit + '</div><div class="cardValue mirror">' + temp[0] + `</div></div>`;
			}
			else if(temp.length = 1)
			{
				document.getElementById("blackjackPlayerHand2").innerHTML += `<div class="hiddenCard"></div>`;
			}
		}
		if(await turn > 0)
		{
			document.getElementById("blackjackPlayerHand2").innerHTML += `<h4 class="turnIndicator"><~</h4>`;
		}
	}
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
async function getSettlement()
{
	let x = -1;
	let url = "/blackjack";
	let data = {
		"getsettlement": sessionStorage.getItem('token')
	}
	const phaseResponse = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( async function(response) 
		{
			x = parseInt(response);
			if(x >= 0)
			{
				await processResults(x);
				await post({"stop":sessionStorage.getItem('token')});
			}
		}
	).catch(error => console.log(error));
	return x;
}

async function processResults(x)
{
//	let DH = document.getElementById("blackjackSettlementDealerHand");
	let PH = document.getElementById("blackjackSettlementPlayerHand");
	let PH2 = document.getElementById("blackjackSettlementPlayerHand2");

	//	0	loss
	//	1	win
	//	2	draw
	//	3	loss loss
	//	4	draw loss
	//	5	loss draw
	//	6	draw draw
	//	7	win loss
	//	8	loss win
	//	9	win win
	if(x >= 0)
	{
		if(x == 0)
		{
			PH.innerHTML = "LOSE";
		}
		else if(x == 1)
		{
			PH.innerHTML = "WIN!";
		}
		else if(x == 2)
		{
			PH.innerHTML = "DRAW";
		}
		else if(x == 3)
		{
			PH.innerHTML = "LOSE";
			PH2.innerHTML = "LOSE";
		}
		else if(x == 4)
		{
			PH.innerHTML = "DRAW";
			PH2.innerHTML = "LOSE";
		}
		else if(x == 5)
		{
			PH.innerHTML = "LOSE";
			PH2.innerHTML = "DRAW";
		}
		else if(x == 6)
		{
			PH.innerHTML = "DRAW";
			PH2.innerHTML = "DRAW";
		}
		else if(x == 7)
		{
			PH.innerHTML = "WIN!";
			PH2.innerHTML = "LOSE";		
		}
		else if(x == 8)
		{
			PH.innerHTML = "LOSE";
			PH2.innerHTML = "WIN";
		}
		else
		{
			PH.innerHTML = "WIN!";
			PH2.innerHTML = "WIN!";
		}
		
		PH.style.display = "block";
		PH2.style.display = "block";
	}
}