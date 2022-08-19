window.addEventListener('load', () => init());

function init()
{
	let logout = document.querySelector("#logout");
	logout.addEventListener("submit", (event) => logoutUser(event));
}

async function logoutUser(event)
{
	event.preventDefault();

	let cookie = decodeURIComponent(document.cookie).split("=");
	let token = cookie[1];

	let url = "/logout";
	let data = {
		"logout": token
	}
	const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
	.then(response => response.text())
	.then( function(response) 
	{ 
		if(response.toString() == "OK")
		{
			window.location.href = "index.html";
			document.cookie = "token=;";
		}
	} )
	.catch(error => console.log(error));
}