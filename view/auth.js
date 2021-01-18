window.addEventListener('load', () => auth());

async function auth()
{
	let url = "/loggedIn";
	let token = sessionStorage.getItem('token');
	
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