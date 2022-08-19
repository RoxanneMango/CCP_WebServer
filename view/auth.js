window.addEventListener('load', () => auth());

// kb14jen8n2ekb14jen8n2e

async function auth()
{
	let url = "/loggedIn";
//	let token = sessionStorage.getItem('token');
	let cookie = decodeURIComponent(document.cookie).split("=");
	let token = cookie[1];
	
	console.log("token : " + token);
	
	if(token == null)
	{
		token = "null";
	}
	
	let data =
	{
		"loggedIn": token
	}
	
	let num = 0;
	
	let response = await fetch(url, {method: 'POST',body: JSON.stringify(data)})
		.then( response => response.text() )
		.then( response => num = parseInt(response.toString()) )
		.catch(error => console.log(error));
	
	if(num == 1)
	{
		window.location.replace("home.html");
	}
}