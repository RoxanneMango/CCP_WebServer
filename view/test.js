window.addEventListener('load', () => test());

function test()
{
	getTest();
	document.getElementById("test").addEventListener("click", getName);
}

async function getTest()
{
	let url = "/test";
	let data =
	{
		"token": sessionStorage.getItem('token')
	}
	
	let response =
	await fetch( url, {
		method: 'GET',
		headers: new Headers({
			'Authorization': 'Basic ' + sessionStorage.getItem('token')
//			'Authorization': 'Basic '+btoa('username:password'), 
		})
	})
	.then( response => response.text() )
	.catch( error => console.log(error) );
	
	console.log("response : " + await response);
}