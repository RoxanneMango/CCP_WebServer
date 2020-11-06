window.addEventListener('load', () => init());

function init()
{
	login.addEventListener("submit", (event) => loginUser(event));
}

async function loginUser(event)
{
	event.preventDefault();
	let formData = new URLSearchParams(new FormData(document.getElementById("login")));

//	if((await loggedIn()) == 0)
//	{	
		let token = Math.random().toString(36).substr(2);
		token += token;

		sessionStorage.setItem('token', token);

		let url = "/login";
		let data = {
			"login": token,
			"username": formData.get("username"),
			"password": formData.get("password")
		}
		const response = await fetch(url, {method: 'POST', body: JSON.stringify(data)})
		.then(response => response.text())
		.then( function(response) 
			{
				if(response.toString() == "OK") 
				{
					window.location.href = "home.html";
				}
				else
				{
					document.getElementById("errorResponse").innerHTML = response.toString();
				}
			}
		)
		.catch(error => console.log(error));
//	}
}