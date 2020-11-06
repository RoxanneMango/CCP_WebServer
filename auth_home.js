window.addEventListener('load', () => auth());

async function auth()
{
	let url = "/loggedIn";
	let data =
	{
		"loggedIn": sessionStorage.getItem('token')
	}
	
	let num = 0;
	
	let response = await fetch(url, {method: 'POST',body: JSON.stringify(data)})
		.then( response => response.text() )
		.then( response => num = parseInt(response.toString()) )
		.catch(error => console.log(error));
	
	if(num == 0)
	{
		window.location.replace("index.html");
	}
}