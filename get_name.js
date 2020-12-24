window.addEventListener('load', () => userInfo());

function userInfo()
{
	getName();
//	document.querySelector(".name").addEventListener("click", () => window.location.href = "user.html");
//	document.querySelector(".name").addEventListener("mouseover", () => document.querySelector(".name").style.cursor = "pointer" );
}

async function getName()
{
	let url = "/getName";
	let data =
	{
		"token": sessionStorage.getItem('token')
	}
	let response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
		.then( response => response.text() )
		.then( (response) => 
			{
				document.querySelector(".name").innerHTML = "~/" + response.toString(); 
			}
		)
		.catch(error => console.log(error));
}