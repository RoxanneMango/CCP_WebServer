window.addEventListener('load', () => init());
function init()
{
	let token = sessionStorage.getItem('token');
	
	let joinB = document.getElementById("blackjackJoinButton");
	let startB = document.getElementById("blackjackStartButton");
	let stopB = document.getElementById("blackjackStopButton");
	
	joinB.addEventListener("click", () => { post({"blackjack":"join","token":token}); }, false);
	startB.addEventListener("click", () => post({"blackjack":"start","token":token}), false);
	stopB.addEventListener("click", () => post({"blackjack":"stop","token":token}), false);
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