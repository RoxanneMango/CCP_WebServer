window.addEventListener('load', () => userBalance());

function userBalance()
{
	getBalance();
}

async function getBalance()
{
	let url = "/getBalance";

	let cookie = decodeURIComponent(document.cookie).split("=");
	let token = cookie[1];

	let data =
	{
		"token": token
	}
	let response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
		.then( response => response.text() )
		.then( (response) => 
			{
				document.querySelector(".userBalance").innerHTML = "$" + parseFloat(response).toFixed(2); 
			}
		)
		.catch(error => console.log(error));
}