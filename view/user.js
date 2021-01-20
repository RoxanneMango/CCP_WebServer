window.addEventListener('load', () => userInit());
async function userInit()
{
	let addBalanceD = document.getElementById("addBalance");
	let userBalanceD = document.getElementById("userBalance");
	document.getElementById("addBalanceButton").addEventListener("click", async function() 
	{
		if(addBalanceD.style.display == "block")
		{
			document.getElementById("errorResponseAddBalance").innerHTML = "";
			document.getElementById("addBalanceForm").reset();
			addBalanceD.style.display = "none";
		}
		else
		{
			userBalanceD.innerHTML = "<h4>balance : " + parseFloat(await fetch("/getBalance",{method: 'POST',body: JSON.stringify({"token" : sessionStorage.getItem('token')})}).then(response => response.text()).catch(error => console.log(error))).toFixed(2) + "</h4>";
			addBalanceD.style.display = "block";
		}
	}, false);
	document.getElementById("addBalanceForm").addEventListener("submit", async function(event)
	{
		await addBalance(event);
	}, false);	
}

async function addBalance(event)
{
	event.preventDefault();
	document.getElementById("errorResponseAddBalance").innerHTML = "";
	let formData = new URLSearchParams(new FormData(document.getElementById("addBalanceForm")));
	let url = "/addBalance";
	let data = {
		"addBalance": sessionStorage.getItem('token'),
		"amount": formData.get("balanceAmount").toString()
	}
	let isOK = false;
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( async function(response) 
		{
			if(response.toString() == "OK")
			{
				console.log("BALANCE_ADDED");
				document.getElementById("userBalance").innerHTML = "<h4>balance : " + parseFloat(await fetch("/getBalance",{method: 'POST',body: JSON.stringify({"token" : sessionStorage.getItem('token')})}).then(response => response.text()).catch(error => console.log(error))).toFixed(2) + "</h4>";
				document.getElementById("addBalanceForm").reset();
				isOK = true;
			}
			else
			{
				document.getElementById("errorResponseAddBalance").innerHTML = response.toString();
			}
		}
	)
	.catch(error => console.log(error));
	return isOK;
}