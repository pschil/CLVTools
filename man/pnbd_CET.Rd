% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{pnbd_CET}
\alias{pnbd_CET}
\alias{pnbd_nocov_CET}
\alias{pnbd_staticcov_CET}
\title{Pareto/NBD: Conditional Expected Transactions}
\usage{
pnbd_nocov_CET(r, alpha_0, s, beta_0, dPeriods, vX, vT_x, vT_cal)

pnbd_staticcov_CET(
  r,
  alpha_0,
  s,
  beta_0,
  dPeriods,
  vX,
  vT_x,
  vT_cal,
  vCovParams_trans,
  vCovParams_life,
  mCov_trans,
  mCov_life
)
}
\arguments{
\item{r}{shape parameter of the Gamma distribution of the purchase process. The smaller r, the stronger the heterogeneity of the purchase process}

\item{alpha_0}{rate parameter of the Gamma distribution of the purchase process}

\item{s}{shape parameter of the Gamma distribution for the lifetime process. The smaller s, the stronger the heterogeneity of customer lifetimes}

\item{beta_0}{rate parameter for the Gamma distribution for the lifetime process.}

\item{dPeriods}{number of periods to predict}

\item{vX}{Frequency vector of length n counting the numbers of purchases.}

\item{vT_x}{Recency vector of length n.}

\item{vT_cal}{Vector of length n indicating the total number of periods of observation.}

\item{vCovParams_trans}{Vector of estimated parameters for the transaction covariates.}

\item{vCovParams_life}{Vector of estimated parameters for the lifetime covariates.}

\item{mCov_trans}{Matrix containing the covariates data affecting the transaction process. One column for each covariate.}

\item{mCov_life}{Matrix containing the covariates data affecting the lifetime process. One column for each covariate.}
}
\value{
Returns a vector containing the conditional expected transactions for the existing
customers in the Pareto/NBD model.
}
\description{
Calculates the expected number of transactions in a given time period based
on a customer's past transaction behavior and the Pareto/NBD model parameters.

\itemize{
\item{\code{pnbd_nocov_CET}}{ Conditional Expected Transactions without covariates}
\item{\code{pnbd_staticcov_CET}}{ Conditional Expected Transactions with static covariates}
}
}
\details{
\code{mCov_trans} is a matrix containing the covariates data of
the time-invariant covariates that affect the transaction process.
Each column represents a different covariate. For every column a gamma parameter
needs to added to \code{vCovParams_trans} at the respective position.

\code{mCov_life} is a matrix containing the covariates data of
the time-invariant covariates that affect the lifetime process.
Each column represents a different covariate. For every column a gamma parameter
needs to added to \code{vCovParams_life} at the respective position.
}
\references{
Schmittlein DC, Morrison DG, Colombo R (1987). \dQuote{Counting Your Customers:
Who-Are They and What Will They Do Next?} Management Science, 33(1), 1-24.

Bachmann P, Meierer M, Naef, J (2021). \dQuote{The Role of Time-Varying Contextual Factors
in Latent Attrition Models for Customer Base Analysis} Marketing Science 40(4). 783-809.

Fader PS, Hardie BGS (2005). \dQuote{A Note on Deriving the Pareto/NBD Model and
Related Expressions.}
URL \url{http://www.brucehardie.com/notes/009/pareto_nbd_derivations_2005-11-05.pdf}.

Fader PS, Hardie BGS (2007). \dQuote{Incorporating time-invariant covariates into the
Pareto/NBD and BG/NBD models.}
URL \url{http://www.brucehardie.com/notes/019/time_invariant_covariates.pdf}.

Fader PS, Hardie BGS (2020). \dQuote{Deriving an Expression for P(X(t)=x) Under
the Pareto/NBD Model.}
URL \url{https://www.brucehardie.com/notes/012/pareto_NBD_pmf_derivation_rev.pdf}
}
